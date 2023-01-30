# -*- coding: utf-8 -*-
"""
Created on Sat Mar 30 11:40:08 2019

@author: DrLC
"""

import pickle
import json
import random
import numpy
from tools.YYJ import *
class Dataset(object):
    
    def __init__(self, seq=[], userid=[], raw=None, label=[], idx2token=[], token2idx={},filenamelist=[],
                 max_len=300, vocab_size=5000, dtype=None,padding=500):
        
        self.__dtype = dtype
        
        self.__vocab_size = vocab_size
        self.__idx2token = idx2token
        self.__token2idx = token2idx
        assert len(self.__idx2token) == self.__vocab_size \
            and len(self.__token2idx) == self.__vocab_size

        self.__max_len = max_len
        self.__seq = []
        self.__raw = []
        self.__label = []
        self.__uid = []
        self.__len = []
        self.__filenamelist=[]
        if raw is None:
            assert len(seq) == len(label)
            raw = [None for i in label]
        else:
            assert len(seq) == len(raw) and len(raw) == len(label)

        for s, r, y ,uid ,name in zip(seq, raw,  label, userid,filenamelist):
            self.__raw.append(r)
            self.__label.append(y)
            self.__uid.append(uid)
            self.__filenamelist.append(name)
            if len(s) > self.__max_len:
                self.__len.append(self.__max_len)
            else:
                self.__len.append(len(s))
            self.__seq.append([])
            for t in s[:self.__max_len]:
                if t >= self.__vocab_size:
                    self.__seq[-1].append(self.__token2idx['<unk>'])
                else:
                    self.__seq[-1].append(t)
            while len(self.__seq[-1]) < self.__max_len:
                self.__seq[-1].append(padding)
        self.__seq = numpy.asarray(self.__seq, dtype=self.__dtype['int'])
        self.__label = numpy.asarray(self.__label, dtype=self.__dtype['int'])
        self.__len = numpy.asarray(self.__len, dtype=self.__dtype['int'])
        self.__size = len(self.__raw)
        
        assert self.__size == len(self.__raw)      \
            and len(self.__raw) == len(self.__seq)  \
            and len(self.__seq) == len(self.__label) \
            and len(self.__label) == len(self.__len)  \
        
        self.__epoch = None
        self.reset_epoch()

    def reset_epoch(self):
        
        self.__epoch = random.sample(range(self.__size), self.__size)
        
    def next_batch(self, batch_size):
        
        batch = {"x": [], "y": [], "l": [], "raw": [],"uid":[], "filenamelist":[], "new_epoch": False}
        assert batch_size <= self.__size
        if len(self.__epoch) < batch_size:
            batch['new_epoch'] = True
            self.reset_epoch()
        idxs = self.__epoch[:batch_size]
        self.__epoch = self.__epoch[batch_size:]
        batch['x'] = numpy.take(self.__seq, indices=idxs, axis=0)
        batch['y'] = numpy.take(self.__label, indices=idxs, axis=0)
        batch['l'] = numpy.take(self.__len, indices=idxs, axis=0)
        batch['uid'] = numpy.take(self.__uid, indices=idxs, axis=0)
        batch['filenamelist'] = numpy.take(self.__filenamelist, indices=idxs, axis=0)
        for i in idxs:
            batch['raw'].append(self.__raw[i])
        return batch
        
    def indices2seq(self, xs, ls):
        
        seq = []
        for x, l in zip(xs, ls):
            seq.append([])
            for t in x[:l]:
                seq[-1].append(self.__idx2token[t])
        return seq
        
    def get_size(self):
        
        return self.__size
        



            
if __name__ == "__main__":
    
    import time
    start_time = time.time()
    #datadingyi
    vocab_size = 2000;max_len=500
    with open("./preprocessdata/result.pkl","rb") as fp:
        predata=pickle.load(fp)
    with open("./preprocessdata/userid.pkl","rb") as fp:
        iddata=pickle.load(fp)
    raw = predata["raw_te"]
    rep = predata["x_te"]
    label = predata["y_te"]
    id = iddata["te"]
    # 截断
    token2idx = {}
    idx2token = predata["idx2txt"][:vocab_size]
    for i, t in zip(range(vocab_size), idx2token):
        token2idx[t] = i
    dataset = Dataset(seq=rep, raw=raw, label=label,userid=id,
                      idx2token=idx2token, token2idx=token2idx,
                      max_len=max_len, vocab_size=vocab_size,
                      dtype={'fp': numpy.float32, 'int': numpy.int32})
    _b = dataset.next_batch(1)
    ceshi(_b['y'])
    ceshi(dataset.get_size())
    p_ause()
    
    
    
    
    