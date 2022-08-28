# -*- coding: utf-8 -*-
import os, sys
import shutil
import tarfile
import pickle
import tqdm
import re
import ast 
from tools.DataLoader import SplitCharacters
import tools.build_dataset as bd
import Levenshtein as LT

def ceshi(a):
    print("~"*80)
    print("数据类型是：",type(a))
    print("数据内容是：")

    print(a)
    try:
        print("数据的大小是：",len(a))
    except:
        print("无法判断数据长度")
    print("~"*80)
# 使用 pycparaser去解析对应的代码
def p_ause():
    a=input()

# 删减对应的注释   
def remove_comment(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)


# 获得对应数据的tokenize      
def tokenize(dir='./data', src='LibTIFF', tgt='result.pkl',uidfile='userid.pkl',done_folder='preprocessdata',
             done_file="token.done", min_len=5,silence=True):
    
    # record the wrong file
    wrongFileRecord = []

    # the datastruct for approach
    data = {'raw': [], "labels": [], "uids": [],"filenamerec":[]}
    processfilename="Preprocessed"
    labelname=sorted(os.listdir(os.path.join(dir, src)))
    for label in labelname:
        if("No" in label):
            oblabel=0
        else:
            oblabel=1
        for filename in tqdm.tqdm(sorted(os.listdir(os.path.join(dir, src, label)))):
            try:
                # 到这一步获取到所有的文件名，label既是对应的两个文件夹名称，对应的可以识别0，1打标
                tokens, uids = [], {}
                opfile=os.path.join(dir, src, label, filename)
                with open(opfile, 'r', encoding='latin1') as _f:
                    text = _f.read()
                    text = remove_comment(text)
                with open(opfile, 'w', encoding='latin1') as _f:
                    _f.write(text)
                with open(opfile, 'r', encoding='latin1') as _f:
                    lines = _f.readlines()
                    file_list = []
                    for line in lines:
                        if line is not ' ' and line is not '\n': # Remove sapce and line-change characters
                            sub_line = line.split()
                            new_sub_line = []
                            for element in sub_line:
                                new_element = SplitCharacters(element)
                                new_sub_line.append(new_element)
                            new_line = ' '.join(new_sub_line)
                            file_list.append(new_line)
                    new_file_list = ' '.join(file_list)
                    tokens = new_file_list.split()

                # 获得变量名
                variables=[]
                with open(os.path.join(dir, src, label,processfilename,"_"+filename+".txt"),'r', encoding='latin1') as fp:
                    lines = fp.readlines()
                    for line in lines:
                        oplist = line.split("\t")
                        if(oplist[0]=="decl"):
                            variables.append(oplist[-1].replace("\n",""))
                
                # 获得变量索引
                for vary in variables:
                    uids[vary]=[]
                    for num,a in enumerate(tokens):
                        score=LT.ratio(a,vary)
                        if(score>0.65):
                            uids[vary].append(num)


                
                if len(tokens) >= min_len:
                    data['labels'].append(oblabel)
                    data['raw'].append(tokens)
                    data['uids'].append(uids)
                    data['filenamerec'].append(label+filename)
            except Exception as e:
                print(e)
                pass
    if data is not None:
         # 处理数据集为训练集和测试集，分割为0.2
        train, test = bd.split(data)
        # 构建对应的词汇表
        idx2txt, txt2idx = bd.build_vocab(train['raw'])

        # 构建对应的索引表
        train_tokens = bd.text2index(train['raw'], txt2idx)
        test_tokens = bd.text2index(test['raw'], txt2idx)


        #  这一步是在做验证嘛？
        uids = []
        for _uids in train["uids"]:
            for _uid in _uids.keys():
                if _uid not in uids:
                    uids.append(_uid)

        result = {"raw_tr": train["raw"], "y_tr": train["labels"],
                "x_tr": train_tokens,"name_tr": train["filenamerec"],
                "raw_te": test["raw"], "y_te": test["labels"],
                "x_te": test_tokens,"name_te": test["filenamerec"],
                "idx2txt": idx2txt, "txt2idx": txt2idx}


        uid = {"tr": train["uids"], "te": test["uids"], "all": uids}

        if not os.path.exists(os.getcwd()+os.sep+done_folder):
            os.makedirs(os.getcwd()+os.sep+done_folder)
        with open(os.getcwd()+os.sep+done_folder+os.sep+tgt, "wb") as f:
            pickle.dump(result, f)
        with open(os.getcwd()+os.sep+done_folder+os.sep+uidfile, "wb") as f:
            pickle.dump(uid, f)
        with open(os.path.join(dir, done_file), "wb") as f:
            pass  
    return result, uid

# tokenize(dir="../data")  
  

