import random
import numpy
import copy
from tools.YYJ import *
from keras.models import load_model, Model
from keras.preprocessing.sequence import pad_sequences
import numpy as np
from sklearn.metrics import classification_report, confusion_matrix
from threading import Thread
import shutil 
import json
import pickle
import time
import os
import yaml
from scipy.stats import norm
from dataset import Dataset
from generate_datastruct import tokenize

from sklearn.metrics import roc_auc_score
from sklearn.metrics import average_precision_score




def caculatedvalue(label,probs):
    result = average_precision_score(label,probs)
    return result


class MHM(object):
    
    def __init__(self, _classifier, _token2idx, _idx2token,  _tokenizer, _attack, Stime,Rtime,_log=-1):

        self.timeset=Rtime
        self.starttime=Stime

        #训练好的分类检测器
        self.classifier = _classifier
        
        #相互映射表
        self.token2idx = _token2idx
        self.idx2token = _idx2token

        #序列化工具
        self.tokenizer = _tokenizer

        #logpath
        self.log = _log

        #攻击范围
        self.attackrange = _attack

    # 获得预测结果
    def getpred(self,probs):
        predicted_classes = []
        for item in probs:
            if item[0] > 0.5:
                predicted_classes.append(1)
            else:
                predicted_classes.append(0) 
        return  predicted_classes
    
    # 核心的MHM方法体 
    def mcmc(self,  _tokens=[], _label=None, _n_candi=30, _userid={},raw_token=[],
             _max_iter=100, _prob_threshold=0.95):

             
        if len(_tokens) == 0 or _label is None:
            return None
        raw_tokens = raw_token.copy()
        uid = _userid

        if len(uid) <= 0:
            print("无法采集变量")
            return {'succ': False, 'tokens': None, 'raw_tokens': None}



        for iteration in range(1, 1+_max_iter):


            currenttime=time.time()
            runningtime=currenttime-self.starttime
            if(runningtime>self.timeset):
                print("超时跳出")
                screen_to_file("time verbose",self.log)
                return {'succ': False, 'tokens': None, 'raw_tokens': None} 
            res = self.__replaceUID(_tokens=raw_tokens, _label=_label, _uid=uid,
                                    _n_candi=_n_candi,
                                    _prob_threshold=_prob_threshold)

            # 这里对应在执行变量名替换函数返回的三种情况：
            # pass：太靠近对应的分类边缘，mhm攻击大概率不会奏效
            # failure：表示该次分类本身即为错误分类不用进行对应的攻击
            if res == "pass":
                print("扰动微弱")
                screen_to_file("weak attack",self.log)
                return {'succ': False, 'tokens': None, 'raw_tokens': None}
            if res == "failure":
                print("模型自身分类失败，无需扰动")
                screen_to_file("model classify failure",self.log)
                return {'succ': False, 'tokens': None, 'raw_tokens': None}
                
            self.__printRes(_iter=iteration, _res=res, _prefix=" >> ")

            # 这里执行的是累计换名的策略
            if res['status'].lower() in ['s', 'a']:
                tokens = res['tokens']
                uid[res['new_uid']] = uid.pop(res['old_uid'])
                # token在这一步换掉以后会继续参与之后的mhm替换策略
                for i in range(len(raw_tokens)):
                    if raw_tokens[i] == res['old_uid']:
                        raw_tokens[i] = res['new_uid']
                if res['status'].lower() == 's':
                    return {'succ': True, 'tokens': tokens,
                            'raw_tokens': raw_tokens,"test_x":res["adv_x"]}
        return {'succ': False, 'tokens': None, 'raw_tokens': None}
        
    # 更换变量名方法体
    def __replaceUID(self, _tokens=[], _label=None, _uid={},
                     _n_candi=30, _prob_threshold=0.95, _candi_mode="random"):
        
        assert _candi_mode.lower() in ["random", "nearby"]

        _tokens = self.tokenizer.texts_to_sequences(_tokens)


        # 选择候选样本中方法
        if _candi_mode == "random":
            # 使用随即方法获得一个候选变量
            selected_uid = random.sample(_uid.keys(), 1)[0]
            candi_token = [selected_uid]
            candi_tokens = [copy.deepcopy(_tokens)]
            candi_labels = [_label]
            for c in random.sample(self.idx2token, _n_candi): 
                if isUID(c):
                    candi_token.append(c)
                    candi_tokens.append(copy.deepcopy(_tokens))
                    candi_labels.append(_label)
                    for i in _uid[selected_uid]:
                        if i >= len(candi_tokens[-1]):
                            break
                        candi_tokens[-1][i] = [self.token2idx[c]]
            
            # 这里是根据原来的token生成30个候选向量，然后30个候选向量做替换，得到对应probs，选出probs最接近的那个进行验证
            _candi_tokens = candi_tokens
            _candi_labels = numpy.asarray(candi_labels)
            suit_tokens=[]
            for i in _candi_tokens:
                suit_tokens.append(transfer(i))
            _candi_x = pad_sequences(suit_tokens, maxlen = model_config["model_max_len"], padding ='post')
            probs = self.classifier.predict(_candi_x, batch_size = 32, verbose=1)
            print("model load succ!")
            pred = self.getpred(probs)
            # 判断更换works or not的循环
            probs = probs.transpose()[0]

            # 获得攻击范围的判断因素
            judge = (np.min(probs[1:])-0.5)
            if judge<0:
                judge = -judge

            if judge > self.attackrange:
                return "pass"
            if pred[0] != _label:
                return "failure"

            # 成功案例返回
            for i in range(len(candi_token)):   
                if pred[i] != _label:
                    print("old {} -> new {}".format(selected_uid,candi_token[i]))
                    return {"status": "s", "alpha": 1, "tokens": candi_tokens[i],
                            "old_uid": selected_uid, "new_uid": candi_token[i],
                            "old_prob": probs[0], "new_prob": probs[i],
                            "old_pred": pred[0], "new_pred": pred[i],
                            "adv_x":_candi_x[i]}

            # 除了第一个(原本预测非候选集)的找一个最小的
            candi_idx = np.argmin(probs[1:]) + 1
            candi_idx = int(candi_idx)

            # 候选集和原预测集的probs做对比计算更换参数alpha
            alpha = (1 - probs[candi_idx] + 1e-10) / (1 - probs[0] + 1e-10)
            if random.uniform(0, 1) > alpha or alpha < _prob_threshold:
                print("old {} -> new {}".format(selected_uid,candi_token[i]))
                return {"status": "r", "alpha": alpha, "tokens": candi_tokens[i],
                        "old_uid": selected_uid, "new_uid": candi_token[i],
                        "old_prob": probs[0], "new_prob": probs[i],
                        "old_pred": pred[0], "new_pred": pred[i]}
            else:
                print("old {} -> new {}".format(selected_uid,candi_token[i]))
                return {"status": "a", "alpha": alpha, "tokens": candi_tokens[i],
                        "old_uid": selected_uid, "new_uid": candi_token[i],
                        "old_prob": probs[0], "new_prob": probs[i],
                        "old_pred": pred[0], "new_pred": pred[i]}
        else:
            # 这里可以选择另一种选取变量的方式
            pass

    # 输出方法体
    def __printRes(self, _iter=None, _res=None, _prefix="  => "):
        
        if _res['status'].lower() == 's':   # Accepted & successful
            print("%s iter %d, SUCC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" % \
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']), flush=True)
            if self.log !=-1:
                screen_to_file("%s iter %d, SUCC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" % \
                    (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                    _res['old_pred'], _res['new_pred'],
                    _res['old_prob'],
                    _res['new_prob'], _res['alpha']),self.log)

        elif _res['status'].lower() == 'r': # Rejected
            print("%s iter %d, REJ. %s => %s (%d => %d, %.5f => %.5f) a=%.3f" % \
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']), flush=True)
            if self.log !=-1:       
                screen_to_file("%s iter %d, REJ. %s => %s (%d => %d, %.5f => %.5f) a=%.3f" % \
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']),self.log)
        elif _res['status'].lower() == 'a': # Accepted
            print("%s iter %d, ACC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" % \
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']), flush=True)
            if self.log !=-1:
                screen_to_file("%s iter %d, ACC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" % \
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']),self.log)


# 清理日志
def cleanlog():
    try:
        shutil.rmtree('./log')  
    except:
        pass
    os.mkdir('./log') 

# 清理结果
def cleanresult():  
    try:
        shutil.rmtree('./result')  
    except:
        pass
    os.mkdir('./result') 



def main(model_config,iteration):

    embedding=model_config["model_name"].split("_")[0]
    if(model_config["MHM"]["attack_range"]>5):
        adv_algr="MHM"
    else:
        adv_algr = "RVMHM"
    print("verbose clean have finished")
    data_base = model_config["data_name"]
    vocab_size = model_config["vocab_size"]
    max_len= model_config["max_len"]

    model_path = model_config["victim_model_path"]+model_config["model_name"]+"_"+model_config["data_name"]+".h5"
    token_path = model_config["tokenizer_path"]+"_"+model_config["data_name"]+"_"+embedding+".pickle"
    adv_test = model_config["adv_test"]+"_"+model_config["data_name"]

    middata_save_path = "mid_data"+os.sep+adv_algr+os.sep+data_base+os.sep
    if not os.path.exists(middata_save_path):
        os.makedirs(middata_save_path)
    log_save_path = "log"+os.sep+adv_algr+os.sep+data_base+os.sep
    if not os.path.exists(log_save_path):
        os.makedirs(log_save_path)
    filelog = log_save_path + os.sep + model_config["model_name"] + "_" + "_log"+ str(iteration)
    middatasave = middata_save_path + model_config["model_name"]+ str(iteration) + ".pickle"
    mid_datadict={
        "pred":[],
        "label":[],
        "advpred":[],   
        "advlabel":[]
    }





    with open(filelog,"w") as f:
        f.write("victimmodel_path："+model_path)

    
     #获得了分好的id和测试数据
    predata, iddata = tokenize(dir="../data",src=data_base)
    raw = predata["raw_te"]
    rep = predata["x_te"]
    label = predata["y_te"]
    id = iddata["te"]
    filename = predata["name_te"]
    
     # 截断
    token2idx = {}
    idx2token = predata["idx2txt"][:vocab_size]
    for i, t in zip(range(vocab_size), idx2token):
        token2idx[t] = i
     # 构建处理的数据结构
    dataset = Dataset(seq=rep, raw=raw, label=label,userid=id,filenamelist=filename,
                      idx2token=idx2token, token2idx=token2idx,
                      max_len=max_len, vocab_size=vocab_size,
                      dtype={'fp': numpy.float32, 'int': numpy.int32})
    print ("DATA LOADED!")


    
     # 进行第一次测试
    print("TEST START")
    screen_to_file("TEST START",filelog)
    test_label=label
    test_raw=raw
    if os.path.isfile(model_path):
        # Load the model and print the model details.
        test_model = load_model(model_path)
        test_model.summary()
    else:
        print("failed load model")
    with open(token_path, "rb") as fp:
        tokenizer = pickle.load(fp)
    total_sequence = tokenizer.texts_to_sequences(test_raw)
     # padding必须和训练的模型一样
    test_x = pad_sequences(total_sequence, maxlen=model_config["model_max_len"], padding='post')
    probs = test_model.predict(test_x, batch_size=32, verbose=1)
    accuracy, predicted_classes = getAccuracy(probs, test_label)

    mid_datadict["pred"] = probs
    mid_datadict["label"] = test_label
    print("Total accuracy: " + str(accuracy))
    screen_to_file("Total accuracy: " + str(accuracy),filelog)
    print("----------------------------------------------------")
    print("The confusion matrix: \n")
    screen_to_file("The confusion matrix:",filelog)
    target_names = ["Non-vulnerable","Vulnerable"] #non-vulnerable->0, vulnerable->1



    print (confusion_matrix(test_label, predicted_classes, labels=[0,1]))  
    screen_to_file(str(confusion_matrix(test_label, predicted_classes, labels=[0,1])),filelog)
    print ("\r\n")
    print (classification_report(test_label, predicted_classes, target_names=target_names))
    screen_to_file(classification_report(test_label, predicted_classes, target_names=target_names),filelog)
    
    AUPRC=caculatedvalue(test_label, probs)
    print("AUPRC: \n")
    print(AUPRC)
    screen_to_file("AUPRC:{} \n".format(AUPRC),filelog)
    print("TEST END")





    victim_models = test_model
    starttime = time.time()
    attacker = MHM(_classifier=victim_models, _token2idx=token2idx, _idx2token=idx2token,
                _tokenizer=tokenizer, _attack=model_config["MHM"]["attack_range"], _log=filelog,Stime=starttime,Rtime=model_config["MHM"]["time"])


    print ("ATTACKER BUILT!")
    screen_to_file("ATTACKER BUILT!",filelog)
    dataset.reset_epoch()
    
    silce_point = int(dataset.get_size())
    test_data={"test_x":[],"test_label":[]}
    data = dataset.next_batch(silce_point)

    n_succ = 0
    # 遍历集合开始
    for iteration in range(len(data["y"])):

        print ("\nEXAMPLE "+str(iteration)+"...")
        screen_to_file("\nEXAMPLE "+str(iteration)+"...",filelog)
        start_time = time.time()
        _res = attacker.mcmc( _tokens=data['x'][iteration],
                                _label=data['y'][iteration], _n_candi=model_config["MHM"]["n_candidate"], _userid=data['uid'][iteration],raw_token=data['raw'][iteration],
                                # _max_iter=200)
                                _max_iter=model_config["MHM"]["max_iter"])


        
        if _res['succ']:
            print ("EXAMPLE "+str(iteration)+" SUCCEEDED!")
            screen_to_file("File "+data['filenamelist'][iteration]+" SUCCEEDED!",filelog)
            print ("time cost = %.2f min" % ((time.time()-start_time)/60))
            screen_to_file("time cost = %.2f min" % ((time.time()-start_time)/60),filelog)
            labeldump=data['y'][iteration]
            xdump=np.array(_res['test_x'])
            test_data['test_x'].append(xdump)
            test_data['test_label'].append(labeldump)
            n_succ += 1
        else:
            print ("EXAMPLE "+str(iteration)+" FAILED.")
            labeldump = data['y'][iteration]
            rawop = list([data['raw'][iteration]])
            total_sequence = tokenizer.texts_to_sequences(rawop)
            # padding必须和训练的模型一样
            rawdump = pad_sequences(total_sequence, maxlen=model_config["model_max_len"], padding='post')
            test_data['test_x'].append(rawdump[0])
            test_data['test_label'].append(labeldump)
        with open(adv_test+".pkl","wb") as f:
            pickle.dump(test_data,f)

    x_te=test_data["test_x"]   
    x_te=np.array(x_te)


    #二阶段非常重要的一个错误
    test_label=test_data['test_label']  
    # 置信度    
    probs = test_model.predict(x_te, batch_size = model_config["batch_size"], verbose=1)
    accuracy, predicted_classes = getAccuracy(probs, test_label)

    mid_datadict["advpred"] = probs
    mid_datadict["advlabel"] = test_label
    with open(middatasave,"wb") as f:
        pickle.dump(mid_datadict,f)
    print("Total accuracy: " + str(accuracy))
    screen_to_file("Total accuracy: " + str(accuracy),filelog)
    print("----------------------------------------------------")
    print("The confusion matrix: \n")
    screen_to_file("The confusion matrix:",filelog)
    target_names = ["Non-vulnerable","Vulnerable"] #non-vulnerable->0, vulnerable->1
    print (confusion_matrix(test_label, predicted_classes, labels=[0,1]))  
    screen_to_file(str(confusion_matrix(test_label, predicted_classes, labels=[0,1])),filelog)
    print ("\r\n")
    print (classification_report(test_label, predicted_classes, target_names=target_names))
    screen_to_file(classification_report(test_label, predicted_classes, target_names=target_names),filelog)
    AUPRC=caculatedvalue(test_label, probs)
    print("AUPRC: \n")
    print(AUPRC)
    screen_to_file("AUPRC:{} \n".format(AUPRC), filelog)
    print("TEST END")



# for i in range(1,9):
#     # with open("./config/analysis.yaml".format(str(i)),"r",encoding="utf-8") as f:
#     # with open("./config/analysisadv.yaml".format(str(i)),"r",encoding="utf-8") as f:    
#     with open("./config/analysis copy {}.yaml".format(str(i)),"r",encoding="utf-8") as f:
#         _res = f.read()
#         model_config = yaml.safe_load(_res)
#     iteration=1
#     for i in range(iteration):
#         main(model_config,i)
#     for i in range(iteration):
#         model_config["MHM"]["attack_range"]=99
#         main(model_config, i)
# for i in range(31,37):
#     # with open("./config/analysis.yaml".format(str(i)),"r",encoding="utf-8") as f:
#     # with open("./config/analysisadv.yaml".format(str(i)),"r",encoding="utf-8") as f:    
#     with open("./config/analysis copy {}.yaml".format(str(i)),"r",encoding="utf-8") as f:
#         _res = f.read()
#         model_config = yaml.safe_load(_res)
#     iteration=1
#     for i in range(iteration):
#         main(model_config,i)
#     for i in range(iteration):
#         model_config["MHM"]["attack_range"]=99
#         main(model_config, i)

for sensitiveiter in range(1):
    with open("./config/analysis.yaml","r",encoding="utf-8") as f:
        _res = f.read()
        model_config = yaml.safe_load(_res)
    model_config["MHM"]["max_iter"]=model_config["MHM"]["max_iter"]-50+sensitiveiter*20
    main(model_config, "max_iter"+str(model_config["MHM"]["max_iter"]))
# for sensitiveiter in range(2,5):
#     with open("./config/analysissensitive.yaml","r",encoding="utf-8") as f:
#         _res = f.read()
#         model_config = yaml.safe_load(_res)
#     model_config["MHM"]["attack_range"]=model_config["MHM"]["attack_range"]-0.1+sensitiveiter*0.04
#     main(model_config, "attack_range"+str(model_config["MHM"]["attack_range"]))