# -*- coding: utf-8 -*-
"""
Created on Tue Aug 27 10:38:49 2019

@author: Daniel Lin

"""
import os
import pickle
import numpy as np
from src.DataLoader import LoadPickleData
import time
"""
Tokenization:
    1. Perform tokenization and save tokenizer.
    2. Load tokenizer.
"""


class Embedding_Model():

    def __init__(self, config):
        self.config = config
        self.tokenizer_path = self.config['embedding_settings']['embedding_model_saved_path']
        self.tokenizer_saved_path = self.config['embedding_settings']['embedding_model_saved_path']
        assert os.path.exists(self.tokenizer_path)
        assert os.path.exists(self.tokenizer_saved_path)
        self.n_workers = self.config['embedding_settings']['n_workers']
        self.seed = self.config['embedding_settings']['seed']
        self.timing = time.strftime("%Y-%m-%d-%H-%M", time.localtime())

    def LoadTokenizer(self, data_list, modelname):
        tokenizer = LoadPickleData(self.tokenizer_path + 'tokenizer_{}.pickle'.format(modelname))
        total_sequences = tokenizer.texts_to_sequences(data_list)
        word_index = tokenizer.word_index

        return total_sequences, word_index


class WordToVec(Embedding_Model):
    ''' Handler for Word2vec training progress...'''

    def __init__(self, config):
        super(WordToVec, self).__init__(config)

        self.wordtovec_size = self.config['embedding_settings']['word2vec']['size']
        self.wordtovec_window = self.config['embedding_settings']['word2vec']['window']
        self.wordtovec_min_count = self.config['embedding_settings']['word2vec']['min_count']
        self.wordtovec_algorithm = self.config['embedding_settings']['word2vec']['algorithm']

    def TrainWordToVec(self, data_list):
        from gensim.models import Word2Vec

        print("----------------------------------------")
        print("Start training the Word2Vec model. Please wait.. ")
        # 2. Train a Vocabulary with Word2Vec -- using the function provided by gensim

        # 修改范围  size=self.wordtovec_size,
        w2vModel = Word2Vec(data_list, workers=self.n_workers, window=self.wordtovec_window,
                            min_count=self.wordtovec_min_count, sg=self.wordtovec_algorithm, seed=self.seed)
        print("Model training completed!")
        print("----------------------------------------")
        print("The trained word2vec model: ")
        print(w2vModel)

        w2vModel.wv.save_word2vec_format(
            self.tokenizer_saved_path + "w2v_model.txt", binary=False)

    def ApplyWordToVec(self, word_index):

        print("-------------------------------------------------------")
        print("Loading trained Word2vec model. ")
        w2v_model = open(self.tokenizer_saved_path + "w2v_model.txt")
        print("The trained word2vec model: ")


        # a dictionary with mapping of a word i.e. 'int' and its corresponding 100 dimension embedding.
        embeddings_index = {}

        # Use the loaded model
        for line in w2v_model:
            if not line.isspace():
                values = line.split()
                word = values[0]
                coefs = np.asarray(values[1:], dtype='float32')
                embeddings_index[word] = coefs
        w2v_model.close()

        print('Found %s word vectors.' % len(embeddings_index))

        embedding_matrix = np.zeros((len(word_index) + 1, self.wordtovec_size))
        for word, i in word_index.items():
            embedding_vector = embeddings_index.get(word)
            if embedding_vector is not None:
                # words not found in embedding index will be all-zeros.
                embedding_matrix[i] = embedding_vector

        return embedding_matrix, self.wordtovec_size



class FastText(Embedding_Model):
    ''' Handler for Word2vec training progress...'''

    def __init__(self, config):
        super(FastText, self).__init__(config)

        self.fasttext_size = self.config['embedding_settings']['fasttext']['size']
        self.fasttext_window = self.config['embedding_settings']['fasttext']['window']
        self.fasttext_min_count = self.config['embedding_settings']['fasttext']['min_count']
        self.fasttext_algorithm = self.config['embedding_settings']['fasttext']['algorithm']

    def TrainFastText(self, data_list):
        from gensim.models import FastText

        print("----------------------------------------")
        print("Start training the FastText model. Please wait.. ")
        # 2. Train a Vocabulary with Word2Vec -- using the function provided by gensim
        ft_Model = FastText(data_list, workers=self.n_workers, window=self.fasttext_window,
                            min_count=self.fasttext_min_count, sg=self.fasttext_algorithm, seed=self.seed)
        print("Model training completed!")
        print("----------------------------------------")
        print("The trained FastText model: ")
        print(ft_Model)

        ft_Model.wv.save_word2vec_format(
            self.tokenizer_saved_path + "ft_model.txt")

    def ApplyFastText(self, word_index):

        #from gensim.models.wrappers import FastText
        print("-------------------------------------------------------")
        print("Loading trained model. ")
        ft_model = open(self.tokenizer_saved_path + "ft_model.txt")
        print("The trained word2vec model: ")
        print(ft_model)

        # a dictionary with mapping of a word i.e. 'int' and its corresponding 100 dimension embedding.
        embeddings_index = {}

        # Use the loaded model
        for line in ft_model:
            if not line.isspace():
                values = line.split()
                word = values[0]
                coefs = np.asarray(values[1:], dtype='float32')
                embeddings_index[word] = coefs
        ft_model.close()
        print('Found %s word vectors.' % len(embeddings_index))

        embedding_matrix = np.zeros((len(word_index) + 1, self.fasttext_size))
        for word, i in word_index.items():
            embedding_vector = embeddings_index.get(word)
            if embedding_vector is not None:
                # words not found in embedding index will be all-zeros.
                embedding_matrix[i] = embedding_vector

        return embedding_matrix, self.fasttext_size


class SPLWord2vec(Embedding_Model):
    def __init__(self, config):
        super(SPLWord2vec, self).__init__(config)
        self.words = "None"
        self.embediding_size = self.config['embedding_settings']['SPL']['embadding_size']
        self.SPL_window = self.config['embedding_settings']['SPL']['window']
        self.SPL_min_count = self.config['embedding_settings']['SPL']['min_count']
        self.SPL_epoch = self.config['embedding_settings']['SPL']['epoch']
        self.SPL_nagetive = self.config['embedding_settings']['SPL']['nagetive_sample']
        self.batch_size = self.config['embedding_settings']['SPL']['batch_size']
        self.save_path = self.config['embedding_settings']['embedding_model_saved_path']

    def getWords(self, opData):
        result = []
        for i in opData:
            for j in i:
                result.append(j)
        return result

    def TrainSPL(self, data_list):
        self.words = self.getWords(data_list)
        model = SPLmain(words=self.words, epoch=self.SPL_epoch, nagetive=self.SPL_nagetive, window=self.SPL_window,
                        batch=self.batch_size, min_count=self.SPL_min_count, embeddingsize=self.embediding_size)
        model.train()
        resultname = self.save_path+"SPL.txt"
        model.save_embedding(resultname)

    def ApplySPL(self, word_index):

        print("-------------------------------------------------------")
        print("Loading trained Word2vec model. ")
        w2v_model = open(self.save_path + "SPL.txt")
        print("The trained SPL model: ")
        print(w2v_model)

        # a dictionary with mapping of a word i.e. 'int' and its corresponding 100 dimension embedding.
        embeddings_index = {}

        # Use the loaded model
        for line in w2v_model:
            if not line.isspace():
                values = line.split()
                word = values[0]
                coefs = np.asarray(values[1:], dtype='float32')
                embeddings_index[word] = coefs
        w2v_model.close()

        print('Found %s word vectors.' % len(embeddings_index))

        embedding_matrix = np.zeros(
            (len(word_index) + 1, self.embediding_size))
        for word, i in word_index.items():
            embedding_vector = embeddings_index.get(word)
            if embedding_vector is not None:
                # words not found in embedding index will be all-zeros.
                embedding_matrix[i] = embedding_vector

        return embedding_matrix, self.embediding_size

class Graph2Vec(Embedding_Model):
    def __init__(self, config):
        super(SPLWord2vec, self).__init__(config)

    def TrainGraph2vec(self, data_list):
        pass

    def ApplyGraph2vec(self):

        print("-------------------------------------------------------")
        print("Loading trained Graph2vec model. ")
        rootpath = os.getcwd() +os.sep + "graph2vec"+ os.sep + "bin"
        datanamelist = getAllFileNameFromRoot(rootpath)
        id_name_list = []
        embedding_list = []
        for name in datanamelist:
            data = pd.read_csv(rootpath + os.sep + name)
            id_list = list(data['type'])
            id_name_list += id_list
            embedding = data[data.columns[1:]]
            embedding = np.mat(embedding)
            embedding = embedding.tolist()
            embedding_list += embedding

        embedding_list = np.mat(embedding_list)
        ceshi(embedding_list[1:10])
        ceshi(id_name_list[1:10])
        ceshi(embedding_list.shape)
        embedding_matrix=embedding_list
        embediding_size=embedding_list.shape(1)
        return embedding_matrix, embediding_size,id_name_list
