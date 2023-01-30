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

from Robust import Attacker


class Rvmhm(Attacker):

    # 初始化模型参数
    def __init__(self,  _config,
                 _runningLog=-1, _sourceDataPath="../data") -> None:
        super().__init__(_config, _sourceDataPath)

        # 配置文件
        self.config = _config

        # 攻击算法参数
        self.token2idx = []
        self.idx2token = []
        self.attackRange = _config["MHM"]["attack_range"]
        self.attackName = "rvmhm"
        self.attackMaxIter = _config["MHM"]["max_iter"]

        # logpath
        log_save_path = "./log"+os.sep+self.attackName + \
            os.sep+_config["data_name"]+os.sep
        if not os.path.exists(log_save_path):
            os.makedirs(log_save_path)
        self.logPath = log_save_path + self.modelName + \
            "_" + self.embedName + "_log" +"_" +str(_runningLog)
        self.midDataSavePath = "mid_data"+os.sep + \
            self.attackName+os.sep+_config["data_name"]+os.sep
        if not os.path.exists(self.midDataSavePath):
            os.makedirs(self.midDataSavePath)

    #Op函数集合
    def __getpred(self, probs):
        predicted_classes = []
        for item in probs:
            if item[0] > 0.5:
                predicted_classes.append(1)
            else:
                predicted_classes.append(0)
        return predicted_classes
    def __transfer(self,listx):
        rlist = []
        for i in listx:
            try:
                rlist.append(i[0])
            except:
                rlist.append(999)
        return rlist


    # MCMC采样主体
    def __mcmc(self,  _tokens=[], _label=None, _n_candi=30, _userid={}, raw_token=[],
               _max_iter=100, _prob_threshold=0.95):

        if len(_tokens) == 0 or _label is None:
            return None
        raw_tokens = raw_token.copy()
        uid = _userid

        if len(uid) <= 0:
            print("无法采集变量")
            return {'succ': False, 'tokens': None, 'raw_tokens': None}
        for iteration in range(1, 1+_max_iter):

            res = self.__replaceUID(_raws=raw_tokens, _label=_label, _uid=uid,
                                    _n_candi=_n_candi,
                                    _prob_threshold=_prob_threshold)
            if res == "pass":
                print("扰动微弱")
                return {'succ': False, 'tokens': None, 'raw_tokens': None}
            if res == "failure":
                print("模型自身分类失败，无需扰动")
                screen_to_file("model classify failure", self.logPath)
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

                    a = self._texts_to_sequences([raw_tokens],self.token2idx)
                    a = pad_sequences(a, maxlen=self.config["model_max_len"], padding='post')
                    probsa = self.classifier.predict(a, batch_size=32, verbose=1)
                    c = [res["adv_x"]]
                    c = np.array(c)
                    probsc = self.classifier.predict(c, batch_size=32, verbose=1)

                    # 本质上对应识别的格式是有问题的

                    result={'succ': True, 'tokens': tokens,
                            'raw_tokens': raw_tokens, "test_x": res["adv_x"]}
                    return result

        return {'succ': False, 'tokens': None, 'raw_tokens': None}

    # 更换获得Markov细致平稳条件主体
    def __replaceUID(self, _raws=[], _label=None, _uid={},
                     _n_candi=30, _prob_threshold=0.95, _candi_mode="random"
                     ):

        assert _candi_mode.lower() in ["random", "nearby"]

        __tokens=self._texts_to_sequences([_raws],self.token2idx)[0]
        _tokens=[]
        for num in __tokens:
            _tokens.append([num])

        # 选择候选样本中方法
        if _candi_mode == "random":
            # 使用随即方法获得一个候选变量
            selected_uid = random.sample(_uid.keys(), 1)[0]
            candi_token = [selected_uid]
            candi_tokens = [copy.deepcopy(_tokens)]
            candi_labels = [_label]
            for c in random.sample(self.idx2token, _n_candi):
                # 这是在筛选变量
                if isUID(c):
                    candi_token.append(c)
                    candi_tokens.append(copy.deepcopy(_tokens))
                    candi_labels.append(_label)
                    for i in _uid[selected_uid]:
                        if i >= len(candi_tokens[-1]):
                            break
                        candi_tokens[-1][i] = [self.token2idx[c]]

            # 这里的candi_token本身就是对应的编码的形式
            # 这里是根据原来的token生成30个候选向量，然后30个候选向量做替换，得到对应probs，选出probs最接近的那个进行验证
            _candi_tokens = candi_tokens
            _candi_labels = numpy.asarray(candi_labels)
            suit_tokens = []
            for i in _candi_tokens:
                suit_tokens.append(self.__transfer(i))


            _candi_x = pad_sequences(
                suit_tokens, maxlen=self.config["model_max_len"], padding='post')

            probs = self.classifier.predict(_candi_x, batch_size=32, verbose=1)
            print("model load succ!")
            pred = self.__getpred(probs)
            # 判断更换works or not的循环
            probs = probs.transpose()[0]

            # 获得攻击范围的判断因素
            judge = (np.min(probs[1:])-0.5)
            if judge < 0:
                judge = -judge

            if judge > self.attackRange:
                return "pass"
            if pred[0] != _label:
                return "failure"

            # 成功案例返回
            for i in range(len(candi_token)):
                if pred[i] != _label:
                    print(
                        "old {} -> new {}".format(selected_uid, candi_token[i]))
                    return {"status": "s", "alpha": 1, "tokens": candi_tokens[i],
                            "old_uid": selected_uid, "new_uid": candi_token[i],
                            "old_prob": probs[0], "new_prob": probs[i],
                            "old_pred": pred[0], "new_pred": pred[i],
                            "adv_x": _candi_x[i]}


            # 除了第一个(原本预测非候选集)的找一个最小的
            candi_idx = np.argmin(probs[1:]) + 1
            candi_idx = int(candi_idx)

            # 候选集和原预测集的probs做对比计算更换参数alpha
            alpha = (1 - probs[candi_idx] + 1e-10) / (1 - probs[0] + 1e-10)

            if random.uniform(0, 1) > alpha or alpha < _prob_threshold:
                print("old {} -> new {}".format(selected_uid, candi_token[i]))
                return {"status": "r", "alpha": alpha, "tokens": candi_tokens[i],
                        "old_uid": selected_uid, "new_uid": candi_token[i],
                        "old_prob": probs[0], "new_prob": probs[i],
                        "old_pred": pred[0], "new_pred": pred[i]}
            else:
                print("old {} -> new {}".format(selected_uid, candi_token[i]))
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
            print("%s iter %d, SUCC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" %
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']), flush=True)
            if self.logPath != -1:
                screen_to_file("%s iter %d, SUCC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" %
                               (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                                _res['old_pred'], _res['new_pred'],
                                   _res['old_prob'],
                                   _res['new_prob'], _res['alpha']), self.logPath)

        elif _res['status'].lower() == 'r':  # Rejected
            print("%s iter %d, REJ. %s => %s (%d => %d, %.5f => %.5f) a=%.3f" %
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']), flush=True)
            if self.logPath != -1:
                screen_to_file("%s iter %d, REJ. %s => %s (%d => %d, %.5f => %.5f) a=%.3f" %
                               (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                                _res['old_pred'], _res['new_pred'],
                                   _res['old_prob'],
                                   _res['new_prob'], _res['alpha']), self.logPath)
        elif _res['status'].lower() == 'a':  # Accepted
            print("%s iter %d, ACC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" %
                  (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                   _res['old_pred'], _res['new_pred'],
                   _res['old_prob'],
                   _res['new_prob'], _res['alpha']), flush=True)
            if self.logPath != -1:
                screen_to_file("%s iter %d, ACC! %s => %s (%d => %d, %.5f => %.5f) a=%.3f" %
                               (_prefix, _iter, _res['old_uid'], _res['new_uid'],
                                _res['old_pred'], _res['new_pred'],
                                   _res['old_prob'],
                                _res['new_prob'], _res['alpha']), self.logPath)

    # 获得变量采集池+分好的数据集
    def __generateDataSet(self) -> Dataset:
        # 获得了分好的id和测试数据
        vocabSize = self.config["vocab_size"]
        maxLen = self.config["max_len"]
        token_path = self.config["tokenizer_path"] + "_" + \
                     self.config["data_name"] + "_" + self.embedName + ".pickle"
        precessingData, idData = tokenize(
            dir="../data", src=self.config["data_name"], tokenizer_path=token_path)
        raw = precessingData["raw_te"]
        rep = precessingData["x_te"]
        label = precessingData["y_te"]
        id = idData["te"]
        filename = precessingData["name_te"]
        # 截断
        token2idx = {}
        idx2token = precessingData["idx2txt"][:vocabSize]
        for i, t in zip(range(vocabSize), idx2token):
            token2idx[t] = i
        self.token2idx = token2idx
        self.idx2token = idx2token
        # 构建处理的数据结构
        dataStructure = Dataset(seq=rep, raw=raw, label=label, userid=id, filenamelist=filename,
                                idx2token=idx2token, token2idx=token2idx,
                                max_len=maxLen, vocab_size=vocabSize,
                                dtype={'fp': numpy.float32, 'int': numpy.int32})
        print("DATA LOADED!")
        return dataStructure

    # 攻击并返回攻击前后的数据集合
    def Attack(self) -> dict:
        res = {
            "ori_raw": [],
            "ori_rep": [],
            "ori_label": [],
            "adv_raw": [],
            "adv_rep": [],
            "adv_label": []
        }
        dataSet = self.__generateDataSet()
        silce_point = int(dataSet.get_size())
        data = dataSet.next_batch(silce_point)

        res["ori_rep"] = data["x"]
        res["ori_label"] = data["y"]
        res["ori_raw"] = data["raw"]

        for iteration in range(len(data["y"])):

            print("\nEXAMPLE "+str(iteration)+"...")
            screen_to_file("\nEXAMPLE "+str(iteration)+"...", self.logPath)
            start_time = time.time()
            _res = self.__mcmc(_tokens=data['x'][iteration],
                               _label=data['y'][iteration], _n_candi=self.config["MHM"][
                                   "n_candidate"], _userid=data['uid'][iteration], raw_token=data['raw'][iteration],
                               # _max_iter=200)
                               _max_iter=self.attackMaxIter)

            if _res['succ']:
                print("EXAMPLE "+str(iteration)+" SUCCEEDED!")
                screen_to_file(
                    "File "+data['filenamelist'][iteration]+" SUCCEEDED!", self.logPath)
                print("time cost = %.2f min" % ((time.time()-start_time)/60))
                screen_to_file("time cost = %.2f min" %
                               ((time.time()-start_time)/60), self.logPath)

                res['adv_rep'].append(np.array(_res['test_x']))
                res['adv_label'].append(data['y'][iteration])
                res['adv_raw'].append(_res['raw_tokens'])
            else:
                print("EXAMPLE "+str(iteration)+" FAILED.")
                labeldump = data['y'][iteration]
                rawop = list([data['raw'][iteration]])
                
                total_sequence = self._texts_to_sequences(rawop,self.token2idx)
                # padding必须和训练的模型一样
                rawdump = pad_sequences(
                    total_sequence, maxlen=self.config["model_max_len"], padding='post')
                res['adv_raw'].append(data['raw'][iteration])
                res['adv_rep'].append(rawdump[0])
                res['adv_label'].append(data['y'][iteration])
        return res


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


if __name__ == "__main__":

    # 加载设置文件
    with open("./config/analysis.yaml", "r", encoding="utf-8") as f:
        _res = f.read()
        modelConfig = yaml.safe_load(_res)

    # 注释运行时间
    runningtime = time.strftime("%a %b %d %H-%M-%S", time.localtime())
    robTest = Rvmhm(_config=modelConfig, _runningLog=runningtime)

    testData = robTest.Attack()

    # 原始测试
    oriPerformance = robTest.Test(
        testData["ori_raw"], testData["ori_label"], robTest.token2idx)
    # 对抗测试
    oriPerformance = robTest.Test(
        testData["adv_raw"], testData["adv_label"],robTest.token2idx)