# -*- coding: utf-8 -*-
"""
Created on Sun Oct 12 00:30:35 2016
@author: Camila Laranjeira
"""
import numpy as np
from sklearn import svm
from sklearn.cross_validation import KFold
from sklearn import preprocessing
import pickle
import os
import sys

# Dataset info
project_root = os.getcwd()
coffee_features = "../features/coffee/"
UCMerced_features = "../dataset/UCMerced/features/"
# "coffee" or "UCMerced"
dataset = "coffee"

def cross_validate(data, labels, classifier):
    scores = []
    if dataset == "coffee":
	kf = [[np.array([1,2,3,4]), np.array([0])], \
	      [np.array([0,2,3,4]), np.array([1])], \
	      [np.array([0,1,3,4]), np.array([2])], \
	      [np.array([0,1,2,4]), np.array([3])], \
	      [np.array([0,1,2,3]), np.array([4])]]
	for k, (train, test) in enumerate(kf):
	    for t in train:
		classifier = classifier.fit(data[t], labels[t])
	    score = classifier.score(data[test], labels[test])
	    scores.append(score)
	    print('{} fold: {:.4f}'.format(k, score))

    else:
    	kf = KFold(labels.size, n_folds=5)

    	for k, (train, test) in enumerate(kf):
            classifier = classifier.fit(data[train], labels[train])
            score = classifier.score(data[test], labels[test])
            scores.append(score)
    	    print('{} fold: {:.4f}'.format(k, score))
        
    return np.mean(scores)
        
if __name__ == '__main__':
    if len(sys.argv) == 2:
	dataset = sys.argv[-1]

    if dataset == "coffee":
        path = os.path.join(project_root, coffee_features)
    else:
	path = os.path.join(project_root, UCMerced_features)

    fold = sorted(os.listdir(path))
    features = []
    labels = []	

    for k, ffold in enumerate(fold):
        features.append(np.loadtxt(os.path.abspath(os.path.join(path, ffold)), delimiter=','))
	labels.append(features[k][:, 0])
	features[k] = np.delete(features[k], 0, 1) #delete first column (labels)
	features[k] = preprocessing.scale(features[k])
    
    clf = svm.LinearSVC(class_weight='balanced', C=1e-3)
    acc = cross_validate(features, labels, clf)
    print('Mean accuracy: {:.4f}'.format(acc))    

