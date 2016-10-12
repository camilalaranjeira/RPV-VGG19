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
coffee_features = "../dataset/coffee/features/"
UCMerced_features = "../dataset/UCMerced/features/"
# "coffee" or "UCMerced"
dataset = "coffee"

def cross_validate(data, labels, classifier):
    """ data: [n_samples, n_features] 
        labels: [n_samples] (value is 0 to n_labels)"""
    kf = KFold(labels.size, n_folds=10)
    scores = []
    for k, (train, test) in enumerate(kf):
        classifier = classifier.fit(data[train], labels[train])
        score = classifier.score(data[test], labels[test])
        scores.append(score)        
        print('{} fold: {:.4f}'.format(k, score))
        
    return np.mean(scores)
    
def store_classifier(classifier):
     # ===== Store classifier ===== #    
    pickle.dump(classifier, open('classifier.p', 'wb'))
        
if __name__ == '__main__':
    if len(sys.argv) == 2:
	dataset = sys.argv[-1]

    if dataset == "coffee":
        path = os.path.join(project_root, coffee_features)
    else:
	path = os.path.join(project_root, UCMerced_features)

    fold = sorted(os.listdir(path))
    fold1 = fold.pop()
    features = np.loadtxt(os.path.abspath(os.path.join(path, fold1, "features.txt")))
    labels = np.loadtxt(os.path.abspath(os.path.join(path, fold1, "labels.txt")))
    for ffold in fold:
    	features = np.append(features, np.loadtxt(os.path.abspath(os.path.join(path, ffold, "features.txt"))), axis=0)
    	labels = np.append(labels, np.loadtxt(os.path.abspath(os.path.join(path, ffold, "labels.txt"))), axis=0)
 
    print features
    print labels   
    features = preprocessing.scale(features)    
 
    clf = svm.LinearSVC(class_weight='balanced', C=1e-4)  
    acc = cross_validate(features, labels, clf)
    print('Mean accuracy: {:.4f}'.format(acc))    
    
    clf = clf.fit(features, labels)
    #store_classifier(clf)
