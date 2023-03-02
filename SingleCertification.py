import random
import numpy
import copy
from YYJ import *
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
import re
from scipy.stats import norm

modelMaxlen=1000
batchSize=32
data="FFmpeg"
embed="fasttext"
# Separate '(', ')', '{', '}', '*', '/', '+', '-', '=', ';', '[', ']' characters.
def SplitCharacters(str_to_split):
    #Character_sets = ['(', ')', '{', '}', '*', '/', '+', '-', '=', ';', ',']
    str_list_str = ''
    
    if '(' in str_to_split:
        str_to_split = str_to_split.replace('(', ' ( ') # Add the space before and after the '(', so that it can be split by space.
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if ')' in str_to_split:
        str_to_split = str_to_split.replace(')', ' ) ') # Add the space before and after the ')', so that it can be split by space.
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if '{' in str_to_split:
        str_to_split = str_to_split.replace('{', ' { ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '}' in str_to_split:
        str_to_split = str_to_split.replace('}', ' } ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '*' in str_to_split:
        str_to_split = str_to_split.replace('*', ' * ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '/' in str_to_split:
        str_to_split = str_to_split.replace('/', ' / ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if '+' in str_to_split:
        str_to_split = str_to_split.replace('+', ' + ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '-' in str_to_split:
        str_to_split = str_to_split.replace('-', ' - ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if '=' in str_to_split:
        str_to_split = str_to_split.replace('=', ' = ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if ';' in str_to_split:
        str_to_split = str_to_split.replace(';', ' ; ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '[' in str_to_split:
        str_to_split = str_to_split.replace('[', ' [ ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if ']' in str_to_split:
        str_to_split = str_to_split.replace(']', ' ] ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if '>' in str_to_split:
        str_to_split = str_to_split.replace('>', ' > ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if '<' in str_to_split:
        str_to_split = str_to_split.replace('<', ' < ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '"' in str_to_split:
        str_to_split = str_to_split.replace('"', ' " ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if '->' in str_to_split:
        str_to_split = str_to_split.replace('->', ' -> ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '>>' in str_to_split:
        str_to_split = str_to_split.replace('>>', ' >> ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if '<<' in str_to_split:
        str_to_split = str_to_split.replace('<<', ' << ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
    
    if ',' in str_to_split:
        str_to_split = str_to_split.replace(',', ' , ')
        str_list = str_to_split.split(' ')
        str_list_str = ' '.join(str_list)
        
    if str_list_str is not '':
        return str_list_str
    else:
        return str_to_split

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

def transpose(list1):
    res=[]
    for i in list1:
        try:
            res.append(i[0])
        except:
            res.append(0)
    return res
def loadData(path):
    with open(path, 'r', encoding='latin1') as _f:
        text = _f.read()
        text = remove_comment(text)
    with open(path, 'w', encoding='latin1') as _f:
        _f.write(text)
    with open(path, 'r', encoding='latin1') as _f:
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
    return tokens
def LoadPickleData(path):
    with open(path, 'rb') as f:
        loaded_data = pickle.load(f)
    return loaded_data

def test(codeInput:str,model,tokenizer):
    print("TEST START")
    test_raw=codeInput
    model.summary()
    total_sequences = tokenizer.texts_to_sequences(test_raw)
    total_sequences = [transpose(total_sequences)]
    # padding必须和训练的模型一样
    test_x = pad_sequences(total_sequences, maxlen=modelMaxlen, padding='post')
    probs = model.predict(test_x, batch_size=batchSize, verbose=1)
    if probs[0][0] > 0.5:
        res="有漏洞"
    else:
        res="无漏洞"
    return res,probs[0][0]

def main():
    code = loadData("./code_Vul.c")
    modelName="FFmpeg_model"
    modelpath = os.getcwd()+os.sep+"result"+os.sep+"models"+os.sep+modelName+".h5"
    model=load_model(modelpath)
    embedName=embed
    tokenizerName=modelName.split("_")[0]+"_"+embedName
    tokenizerNamePath=os.getcwd()+os.sep+"embedding"+os.sep+"tokenizer_"+tokenizerName+".pickle"
    tokenizer=LoadPickleData(tokenizerNamePath)
    res,probs=test(code,model,tokenizer)
    print("结果是:",res)
    print("DNN网络输出",probs)
main()