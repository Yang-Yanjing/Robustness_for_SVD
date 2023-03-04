import itertools
import glob
import numpy as np
import os
import pickle
import random

from nltk import word_tokenize
import torch
from torch import nn
import torch.nn.functional as F
from torch.nn.utils.rnn import pad_sequence
from tqdm import tqdm

import attacks
import data_util
import ibp
import vocabulary

COUNTER_FITTED_FILE = 'IBP/encoder/ft_model.txt'


class RawClassificationDataset(data_util.RawDataset):
  """
  Dataset that only holds x,y as (str, str) tuples
  """
  def get_word_set(self, attack_surface):
    with open(COUNTER_FITTED_FILE) as f:
      counter_vocab = set([line.split(' ')[0] for line in f])
    word_set = set()
    for x, y in self.data:
      words = [w.lower() for w in x.split(' ')]
      for w in words:
        word_set.add(w)
      try:
        swaps = attack_surface.get_swaps(words)
        for cur_swaps in swaps:
          for w in cur_swaps:
            word_set.add(w)
      except KeyError:
        # For now, ignore things not in attack surface
        # If we really need them, later code will throw an error
        pass
    return word_set & counter_vocab

class CodeClassificationDataset(data_util.ProcessedDataset):
  """
  Dataset that holds processed example dicts
  """
  @classmethod
  def from_raw_data(cls, raw_data, vocab, attack_surface=None, truncate_to=None, 
                    downsample_to=None, downsample_shard=0):
    if downsample_to:
      raw_data = raw_data[downsample_shard * downsample_to:(downsample_shard+1) * downsample_to]
    examples = []
    for x, y in raw_data:
      all_words = [w.lower() for w in x.split()]
      if attack_surface:
        all_swaps = attack_surface.get_swaps(all_words)
        words = [w for w in all_words if w in vocab]
        swaps = [s for w, s in zip(all_words, all_swaps) if w in vocab]
        choices = [[w] + cur_swaps for w, cur_swaps in zip(words, swaps)]
      else:
        words = [w for w in all_words if w in vocab]  # Delete UNK words
      if truncate_to:
        words = words[:truncate_to]
      word_idxs = [vocab.get_index(w) for w in words]
      x_torch = torch.tensor(word_idxs).view(1, -1, 1) # (1, T, d)
      if attack_surface:
        choices_word_idxs = [
          torch.tensor([vocab.get_index(c) for c in c_list], dtype=torch.long) for c_list in choices
        ]
        if any(0 in c.view(-1).tolist() for c in choices_word_idxs):
          #raise ValueError("UNK tokens found")
          print("UNK tokens found")
        choices_torch = pad_sequence(choices_word_idxs, batch_first=True).unsqueeze(2).unsqueeze(0) # (1, T, C, 1)
        choices_mask = (choices_torch.squeeze(-1) != 0).long()  # (1, T, C)
      else:
        choices_torch = x_torch.view(1, -1, 1, 1) # (1, T, 1, 1)
        choices_mask = torch.ones_like(x_torch.view(1, -1, 1))
      mask_torch = torch.ones((1, len(word_idxs)))
      x_bounded = ibp.DiscreteChoiceTensor(x_torch, choices_torch, choices_mask, mask_torch)
      y_torch = torch.tensor(y, dtype=torch.float).view(1, 1)
      lengths_torch = torch.tensor(len(word_idxs)).view(1)
      examples.append(dict(x=x_bounded, y=y_torch, mask=mask_torch, lengths=lengths_torch))
    return cls(raw_data, vocab, examples)

  @staticmethod
  def example_len(example):
    return example['x'].shape[1]

  @staticmethod
  def collate_examples(examples):
    """
    Turns a list of examples into a workable batch:
    """
    if len(examples) == 1:
      return examples[0]
    B = len(examples)
    max_len = max(ex['x'].shape[1] for ex in examples)
    x_vals = []
    choice_mats = []
    choice_masks = []
    y = torch.zeros((B, 1))
    lengths = torch.zeros((B, ), dtype=torch.long)
    masks = torch.zeros((B, max_len))
    for i, ex in enumerate(examples):
      x_vals.append(ex['x'].val)
      choice_mats.append(ex['x'].choice_mat)
      choice_masks.append(ex['x'].choice_mask)
      cur_len = ex['x'].shape[1]
      masks[i, :cur_len] = 1
      y[i, 0] = ex['y']
      lengths[i] = ex['lengths'][0]
    x_vals = data_util.multi_dim_padded_cat(x_vals, 0).long()
    choice_mats = data_util.multi_dim_padded_cat(choice_mats, 0).long()
    choice_masks = data_util.multi_dim_padded_cat(choice_masks, 0).long()
    return {'x': ibp.DiscreteChoiceTensor(x_vals, choice_mats, choice_masks, masks),
            'y': y, 'mask': masks, 'lengths': lengths}
  

class VulnerbleCodeDataset(CodeClassificationDataset):
  """
  Dataset that holds the IMDB sentiment classification data
  """
  @classmethod
  def read_text(cls, imdb_dir, split):
    if split == 'test':
      subdir = 'test'
    else:
      subdir = 'train'
    with open(os.path.join(imdb_dir, subdir, 'imdb_%s_files.txt' % split)) as f:
      filenames = [line.strip() for line in f]
    data = []
    num_words = 0
    for fn in tqdm(filenames):
      label = 1 if fn.startswith('pos') else 0
      with open(os.path.join(imdb_dir, subdir, fn),encoding="utf-8") as f:
        # try:
        #   x_raw = f.readlines()[0].strip().replace('<br />', ' ')
        # except:
        #   print("try encoding utf-8")
        #   with open(os.path.join(imdb_dir, subdir, fn),encoding="utf-8") as Exc:
        #     x_raw = Exc.readlines()[0].strip().replace('<br />', ' ')
        x_raw = f.readlines()[0].strip().replace('<br />', ' ')
        x_toks = word_tokenize(x_raw)
        num_words += len(x_toks)
        data.append((' '.join(x_toks), label))
    num_pos = sum(y for x, y in data)
    num_neg = sum(1 - y for x, y in data)
    avg_words = num_words / len(data)
    print('Read %d examples (+%d, -%d), average length %d words' % (
        len(data), num_pos, num_neg, avg_words))
    return data

  @classmethod
  def get_raw_data(cls, imdb_dir, test=False):
    train_data = cls.read_text(imdb_dir, 'train')
    if test:
      dev_data = cls.read_text(imdb_dir, 'test')
    else:
      dev_data = cls.read_text(imdb_dir, 'dev')
    return RawClassificationDataset(train_data, dev_data)
  


  # 其实就是凑4个数据集
def load_datasets(device, opts):

    """
    Loads text classification datasets given opts on the device and returns the dataset.
    If a data cache is specified in opts and the cached data there is of the same class
        as the one specified in opts, uses the cache. Otherwise reads from the raw dataset
        files specified in OPTS.
    Returns:
        - train_data:  EntailmentDataset - Processed training dataset
        - dev_data: Optional[EntailmentDataset] - Processed dev dataset if raw dev data was found or
            dev_frac was specified in opts
        - word_mat: torch.Tensor
        - attack_surface: AttackSurface - defines the adversarial attack surface
    """


    # 首先使用对象去实例化对应得数据集接口
    data_class = VulnerbleCodeDataset

    # 定义字词替换得攻击面（这里应该是可以获取到
    attack_surface = attacks.WordSubstitutionAttackSurface.from_file(opts.neighbor_file)

    print('Reading dataset.')
    raw_data = data_class.get_raw_data(opts.imdb_dir, test=opts.test)
    word_set = raw_data.get_word_set(attack_surface)
    vocab, word_mat = vocabulary.Vocabulary.read_word_vecs(word_set, opts.glove_dir, opts.glove, device)


    train_data = data_class.from_raw_data(raw_data.train_data, vocab, attack_surface,
                                            downsample_to=opts.downsample_to,
                                            downsample_shard=opts.downsample_shard,
                                            truncate_to=opts.truncate_to)
    
    dev_data = data_class.from_raw_data(raw_data.dev_data, vocab, attack_surface,
                                        downsample_to=opts.downsample_to,
                                        downsample_shard=opts.downsample_shard,
                                        truncate_to=opts.truncate_to)


    return train_data, dev_data, word_mat, attack_surface