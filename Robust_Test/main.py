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
from mhm import Rvmhm

def chooseModel(model_path=os.pardir+os.sep+"result"+os.sep+"models"+os.sep+"data"+os.sep,model_info):
    respath = []
    precision_limit = 0.90
    loss_limit = 0.3
    model_load_path = model_path+model_info
    for filename in tqdm.tqdm(sorted(os.listdir(model_load_path)):
        opername = filename.split("_")
        if opername[3]>precision_limit and opername[4]<loss_limit:
            respath.append(model_load_path)
            respath.append(filename)
            return respath
    return 0




if __name__ == "__main__":

    # 加载设置文件
    with open("./config/analysis.yaml", "r", encoding="utf-8") as f:
        _res = f.read()
        modelConfig = yaml.safe_load(_res)
    chosen_model = chooseModel
    if chosen_model != 0 :
        modelConfig["victim_model_path"] = chosen_model[0]
        

    

    # # 注释运行时间
    # runningtime = time.strftime("%a %b %d %H-%M-%S", time.localtime())
    # robTest = Rvmhm(_config=modelConfig, _runningLog=runningtime)

    # testData = robTest.Attack()

    # # 原始测试
    # oriPerformance = robTest.Test(
    #     testData["ori_raw"], testData["ori_label"], robTest.token2idx)
    # # 对抗测试
    # oriPerformance = robTest.Test(
    #     testData["adv_raw"], testData["adv_label"],robTest.token2idx)