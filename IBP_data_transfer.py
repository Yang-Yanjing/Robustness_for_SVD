import argparse
import glob
import json
import os
import random
import sys

import numpy as np
import torch

from tqdm import tqdm

import IBP.data_util
import IBP.entailment
import IBP.text_classification
import IBP.vocabulary
from IBP.YYJ import ceshi


random.seed(OPTS.rng_seed)
np.random.seed(OPTS.rng_seed)
torch.manual_seed(OPTS.torch_seed)
torch.backends.cudnn.deterministic = True
device = torch.device('cuda') if torch.cuda.is_available() else torch.device('cpu')
