import os
import numpy as np
import matplotlib.pyplot as plt
import pickle


def testbatch():
    dataname="LibPNG"
    # Asterisk, LibPNG, FFmpeg
    rootpath=os.getcwd()+os.sep+"SPL"+os.sep+"models"+os.sep+"data"+os.sep+dataname
    modellist=getAllFileNameFromRoot(rootpath)
    for modelpath in modellist:
        ceshi(modelpath)
        os.system("Python main.py --config config\\config.yaml --test --trained_model"+" "
                  +rootpath+os.sep+modelpath)


# 测试一次
def testonce():
    # 模型存放处
    modelname="model_"
    rootpath = os.getcwd()+os.sep+"result"+os.sep+"models"+os.sep+modelname
    os.system("python main.py --config config/config.yaml --test --trained_model"+" "
                  +rootpath+".h5")

# 训练一次
def trainonce():
    for i in range(5,6):
        os.system("python main.py --config config/config{}.yaml".format(i))
        print("Number"+str(i)+"have done")

# testonce()


# trainonce()
# os.system("python main.py --config config/config.yaml")
print("YYJ")



