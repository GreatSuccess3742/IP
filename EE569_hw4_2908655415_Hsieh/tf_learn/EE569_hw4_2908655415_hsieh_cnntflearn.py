#Name:		Hsiang-Chih,Hsieh
#USC ID:		2908655415
#USC Email:		hsie626@usc.edu
#Submission Date:	4/20/2017
#References: https://github.com/tflearn/tflearn/blob/master/examples/images/convnet_cifar10.py

# -*- coding: utf-8 -*-

""" Convolutional network applied to CIFAR-10 dataset classification task.
References:
    Learning Multiple Layers of Features from Tiny Images, A. Krizhevsky, 2009.
Links:
    [CIFAR-10 Dataset](https://www.cs.toronto.edu/~kriz/cifar.html)
"""
from __future__ import division, print_function, absolute_import

import tflearn
from tflearn.data_utils import shuffle, to_categorical
from tflearn.layers.core import input_data, dropout, fully_connected
from tflearn.layers.conv import conv_2d, max_pool_2d
from tflearn.layers.estimator import regression
from tflearn.data_preprocessing import ImagePreprocessing
from tflearn.data_augmentation import ImageAugmentation
import scipy
import sys
from sklearn.cluster import KMeans
import tensorflow as tf
import numpy as np

# Data loading and preprocessing
from tflearn.datasets import cifar10
(X, Y), (X_test, Y_test) = cifar10.load_data(dirname="..")
X, Y = shuffle(X, Y)
Y = to_categorical(Y, 10)
Y_test = to_categorical(Y_test, 10)

# Real-time data preprocessing
img_prep = ImagePreprocessing()
img_prep.add_featurewise_zero_center()
img_prep.add_featurewise_stdnorm()

# Real-time data augmentation
img_aug = ImageAugmentation()
img_aug.add_random_flip_leftright()
img_aug.add_random_rotation(max_angle=25.)


'''----------------add kmeans here----------------------------'''


# Convolutional network building
network = input_data(shape=[None, 32, 32, 3],
                     data_preprocessing=img_prep,
                     data_augmentation=img_aug)

network = conv_2d(network, 6, 5, activation='relu')
 
network = max_pool_2d(network, 2)
#network = conv_2d(network, 28, 6, activation='relu')
network = conv_2d(network, 16, 5, activation='relu')
network = max_pool_2d(network, 2)
network = fully_connected(network, 120, activation='relu')
network = fully_connected(network, 84, activation='relu')
#network = dropout(network, 0.5)
network = fully_connected(network, 10, activation='softmax')
network = regression(network, optimizer='adam',
                     loss='categorical_crossentropy',
                     learning_rate=0.001)

# Train using classifier
model = tflearn.DNN(network, tensorboard_verbose=0, tensorboard_dir="logs")
model.fit(X, Y, n_epoch=10, shuffle=True, validation_set=(X_test, Y_test),
          show_metric=True, batch_size=128, run_id='cifar10_cnn_epoch10_data_aug_batch_128')