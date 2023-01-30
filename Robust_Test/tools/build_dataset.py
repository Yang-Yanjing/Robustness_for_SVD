# -*- coding: utf-8 -*-
"""
Created on Tue Mar  3 16:50:59 2020

@author: DrLC
"""

import os, sys
import pickle, gzip
import random
# 进度条
import tqdm

# 对数据集的分割
def split(data, test_ratio=0.2,
                seed=1815):
                # seed=random.randint(0,5000)):
    print(seed)
    random.seed(seed)
    n_test = int(test_ratio * len(data['labels']))
    rand_idx = random.sample(range(len(data['labels'])), len(data['labels']))
    test = {"raw": [], "labels": [], "uids": [],"filenamerec":[]}
    train = {"raw": [], "labels": [], "uids": [],"filenamerec":[]}
    for i in rand_idx[:n_test]:
        test['raw'].append(data['raw'][i])
        test['labels'].append(data['labels'][i])
        test['uids'].append(data['uids'][i])
        test['filenamerec'].append(data['filenamerec'][i])
    for i in rand_idx[n_test:]:
        train['raw'].append(data['raw'][i])
        train['labels'].append(data['labels'][i])
        train['uids'].append(data['uids'][i])
        train['filenamerec'].append(data['filenamerec'][i])
    return train, test

# 构建词汇表
def build_vocab(codes,tokenizer_path):
    
    vocab_cnt = {"<str>": 0, "<char>": 0, "<int>": 0, "<fp>": 0}
    for c in tqdm.tqdm(codes):
        for t in c:
            if t[0] == '"' and t[-1] == '"':
                vocab_cnt["<str>"] += 1
            elif t[0] == "'" and t[-1] == "'":
                vocab_cnt["<char>"] += 1
            elif t[0] in "0123456789.":
                if 'e' in t.lower():
                    vocab_cnt["<fp>"] += 1
                elif '.' in t:
                    if t == '.':
                        if t not in vocab_cnt.keys():
                            vocab_cnt[t] = 0
                        vocab_cnt[t] += 1
                    else:
                        vocab_cnt["<fp>"] += 1
                else:
                    vocab_cnt["<int>"] += 1
            elif t in vocab_cnt.keys():
                vocab_cnt[t] += 1
            else:
                vocab_cnt[t] = 1
    vocab_cnt = sorted(vocab_cnt.items(), key=lambda x:x[1], reverse=True)
    idx2txt = ["<unk>"] + [it[0] for it in vocab_cnt]
    txt2idx = {}
    for idx in range(len(idx2txt)):
        txt2idx[idx2txt[idx]] = idx
    characters = ["<unk>"] + [it[0] for it in vocab_cnt]



    with open(tokenizer_path, "rb") as fp:
        tokenizer = pickle.load(fp)
    index = tokenizer.texts_to_sequences(characters)
    text2idx={}
    idx2text = []
    for num,idx in enumerate(index):
        if(len(idx)>0):
            text2idx[characters[num]]=idx[0]
    value_order = sorted(text2idx.items(), key=lambda x: x[1], reverse=False)
    for i in value_order:
        txt2idx[i[0]] = i[1]
    flag = sorted(txt2idx.items(), key=lambda x: x[1], reverse=False)
    for i in flag:
        idx2text.append(i[0])
    text2idx = txt2idx

    return idx2txt, txt2idx
    

# 构建索引表
def text2index(codes, txt2idx):
    
    codes_idx = []
    for c in tqdm.tqdm(codes):
        codes_idx.append([])
        for t in c:
            if t[0] == '"' and t[-1] == '"':
                codes_idx[-1].append(txt2idx["<str>"])
            elif t[0] == "'" and t[-1] == "'":
                codes_idx[-1].append(txt2idx["<char>"])
            elif t[0] in "0123456789.":
                if 'e' in t.lower():
                    codes_idx[-1].append(txt2idx["<fp>"])
                elif '.' in t:
                    if t == '.':
                        codes_idx[-1].append(txt2idx[t])
                    else:
                        codes_idx[-1].append(txt2idx["<fp>"])
                else:
                    codes_idx[-1].append(txt2idx["<int>"])
            elif t in txt2idx.keys():
                codes_idx[-1].append(txt2idx[t])
            else:
                codes_idx[-1].append(txt2idx["<unk>"])
    return codes_idx

    
if __name__ == "__main__":
    
    import mytoken as tk
    from pattern import StmtInsPos, DeclInsPos

    
    random.seed(1726)
    
    dir = './tmp'
    tgt = './YYJ_data/oj.pkl.gz'
    symtab = './YYJ_data/oj_uid.pkl.gz'
    done_file = 'dataset.done'
    inspos_file = './YYJ_data/oj_inspos.pkl.gz'
    number=20

    with open("./tmp/{}tokenized.pkl".format(number),"rb") as f:
        d = pickle.load(f)
    print("data load")
    if d is not None:
        train, test = split(d)
        idx2txt, txt2idx = build_vocab(train['raw'])
        train_tokens = text2index(train['raw'], txt2idx)
        test_tokens = text2index(test['raw'], txt2idx)
        uids = []
        for _uids in train["uids"]:
            for _uid in _uids.keys():
                if _uid not in uids:
                    uids.append(_uid)
        if not os.path.isfile(os.path.join(dir, done_file)):
            data = {"raw_tr": train["raw"], "y_tr": train["labels"],
                    "x_tr": train_tokens,
                    "raw_te": test["raw"], "y_te": test["labels"],
                    "x_te": test_tokens,
                    "idx2txt": idx2txt, "txt2idx": txt2idx}
            uid = {"tr": train["uids"], "te": test["uids"], "all": uids}
            with gzip.open(tgt, "wb") as f:
                pickle.dump(data, f)
            with gzip.open(symtab, "wb") as f:
                pickle.dump(uid, f)
            with open(os.path.join(dir, done_file), "wb") as f:
                pass
            stmt_poses_tr = [StmtInsPos(tr) for tr in tqdm(train['raw'])]
            stmt_poses_te = [StmtInsPos(te) for te in tqdm(test['raw'])]
            decl_poses_tr = [DeclInsPos(tr) for tr in tqdm(train['raw'])]
            decl_poses_te = [DeclInsPos(te) for te in tqdm(test['raw'])]
            inspos = {"stmt_tr": stmt_poses_tr, "stmt_te": stmt_poses_te, 
                        "decl_tr": decl_poses_tr, "decl_te": decl_poses_te}
            with gzip.open(inspos_file, "wb") as f:
                pickle.dump(inspos, f)