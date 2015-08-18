# -*- coding: utf-8 -*-
"""
Created on Fri Aug 14 07:28:11 2015

@author: Benjamin Woodward

This module contains the definition for the fish object class and fish object list
class, which will be used to keep statistics on the fish in a video. 
"""

class Fish(object):
    def __init__(self,fishType=0, subType=0, frameCounted=0):
        self.__fishLabels = dict([(0,'Unknown'),(1,'Flounder'),(2,'Skate'),(3,'Cod'),(4,'Haddock')])        
        self.fishType = self.setFishType(fishType)
        self._subTypeLabels = self.__setSubTypeLabels()
        self.subType = self.setSubType(subType)
        self.frameCounted = frameCounted
  
    def setFishType(self,fishType=0):
        #accessor to set fish type
        self.fishType = self.fishLabels[fishType]
        self._fishIndex = fishType
        
    def setSubType(self,subType=0):
        #accessor to set fish subtype
        self.subType = self._subTypeLabels[subType]
        
    def setFrameCounted(self,frameCounted=0):
        #accessor to set frame where fish is counted
        self.frameCounted = frameCounted
        
    def getFishType(self):
        #accessor to get fish type
        return self.fishType
    
    def getFishIndex(self):
        #accessor to get fish index
        return self.fishIndex
        
    def getFishSubType(self):
        #accessor to get fish subtype
        return self.subType
        
    def getFrameCounted(self):
        #accessor to get frame where fish was counted
        return self.frameCounted
        
    def __setSubTypeLabels(self):
        if self.fishType == 'Unknown':
            subTypeLabelDict = []
        elif self.fishType == 'Flounder':
            subTypeLabelDict = dict([(0,'Unknown'),(1,'subType1'),(2,'subType2')])
        elif self.fishType == 'Skate':
            subTypeLabelDict = dict([(0,'Unknown'),(1,'subType1'),(2,'subType2')])
        elif self.fishType == 'Cod':
            subTypeLabelDict = dict([(0,'Unknown'),(1,'subType1'),(2,'subType2')])
        elif self.fishType == 'Haddock':
            subTypeLabelDict = dict([(0,'Unknown'),(1,'subType1'),(2,'subType2')])
        self._subTypeLabels = subTypeLabelDict

class FishList(object):
    def __init__(self):
        #initialize empty list
        self.data = []        
#Need method for adding entry
#Need method for sorting all entries by time, type, etc.
#Need to define properties
#Need method to read list from .csv
