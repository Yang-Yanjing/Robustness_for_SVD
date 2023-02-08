# -*- coding: utf-8 -*-
"""
Created 2023/1/7

@author: Enoch_Young
"""


import random
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

from sklearn.metrics import average_precision_score

#nameRegular:camel
class Attacker(object):
    def __init__(self, _config,
                _sourceDataPath="../data") -> None:
        
        # 待攻击对象初始化
        self.embedName = _config["model_name"].split("_")[0]
        self.modelName = _config["model_name"].split("_")[1]
        self.modVocab_size = _config["vocab_size"]
        self.modMax_len = _config["max_len"]
        model_path = _config["victim_model_path"] + \
            _config["model_name"]+"_"+_config["data_name"]+".h5"
        token_path = _config["tokenizer_path"]+"_" + \
            _config["data_name"]+"_"+self.embedName+".pickle"
        # 模型本体
        self.classifier = load_model(model_path)
        # 序列化本体
        # with open(token_path, "rb") as fp:
        #     self.tokenizer = pickle.load(fp)
        #模型参数设置文件（起到全局变量控制作用）
        self.config = _config

        self.midData = {
            "originRaws":[],
            "originProbs":[],
            "originLabels":[],
            "advRaws":[],
            "advProbs":[],
            "advLabels":[]
        }

    def _texts_to_sequences(self, test_raw, txt2idx):
        sequences=[]
        for _raw in test_raw:
            seq=[]
            for item in _raw:
                try:
                    seq.append(txt2idx[item])
                except:
                    seq.append(0)
            sequences.append(seq)
        return sequences

    def _caculateAUPRC(self,label,probs):
        result = average_precision_score(label,probs)
        return result

    def Test(self,_testRaw, _testLabel, txt2idx:dict):
        print("TEST START")
        test_label=_testLabel
        test_raw=_testRaw
        self.classifier.summary()

        total_sequence = self._texts_to_sequences(test_raw,txt2idx)


        # padding必须和训练的模型一样
        test_x = pad_sequences(total_sequence, maxlen=self.config["model_max_len"], padding='post')

        probs = self.classifier.predict(test_x, batch_size=self.config["batch_size"], verbose=1)
        accuracy, predicted_classes = getAccuracy(probs, test_label)
        self.midData["originProbs"] = probs
        self.midData["originLabels"] = test_label
        print("Total accuracy: " + str(accuracy))
        print("----------------------------------------------------")
        print("The confusion matrix: \n")
        target_names = ["Non-vulnerable","Vulnerable"] #non-vulnerable->0, vulnerable->1
        print (confusion_matrix(test_label, predicted_classes, labels=[0,1]))  
        print ("\r\n")
        report=classification_report(test_label, predicted_classes, target_names=target_names)
        print (report)
        print("AUPRC: \n")
        AUPRC=self._caculateAUPRC(test_label,probs)
        print(AUPRC)
        print("TEST END")
        res={
            "accuracy":str(accuracy),
            "AUPRC":str(AUPRC),
            "report":report
        }
        return res 

    def Attack(self) -> dict:
        # 对应写完整的攻击过程返回对应test_x,以及test_y
        # 这里理论上来说应该是要给text_raw是最合适的、但是这里tokenizer和tokenize的关系不明确这里无法直接换算
        # 这也是这个研究最大的问题
        # 鲁棒性问题
        pass



        
        