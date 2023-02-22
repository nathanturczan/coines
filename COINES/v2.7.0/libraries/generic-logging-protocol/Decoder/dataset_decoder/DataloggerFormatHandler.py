()#!/usr/bin/env python
# coding: utf-8

# ## Dataset Conversion
# The data conversion tools scans for binary files in the dataset repository and converts them to csv-files.
# 1. define path to dataset folder
# 2. run conversion
#  
# author:    Sergej Scheiermann (BST/ESW4)
#
###########################################################

import os
import re
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

import json
import datetime
import sys
sys.path.append('../GLP_Decoder')
# import the data logger format converter
import GLP_Decoder

############################################################
# Datalogger Format Handler
class DatasetFormatHandler():
    """
    This class implements the datalogger format handler.
    
    ...

    Constants
    ----------
    __META_PATH__ : str
        meta-info path location.
    __META_ACTIVITY__ : str
        activity meta file name.
    __META_ALGORESULTS__ : str
        algoresults file contains meta data about algorithm results (not used today).
    __META_FILEINFO__ : str
        fileinfo file contains meta data about logged files.
    __META_USERINFO__ : str
        userinfo contains user specific meta data.
    __META_DEVICEINFO__ : str
        deviceinfo file contains infrmation about device configurations for every logged file.
    
    Methods
    -------
    get_version()
        prints version of the datalogger handler.
    get_binaries(directory)
        returns list of binary files containing full path and binary name in the dataset directory.
    
    """
    
    __version__ = "V_1_0_0"   # version information
    
    __META_PATH__ = '00_meta\\'
    __META_ACTIVITY__ = 'activity.txt'
    __META_ALGORESULTS__ = 'algoResults.txt'
    __META_FILEINFO__ = 'fileinfo.txt'
    __META_USERINFO__ = 'userinfo.txt'
    __META_DEVICEINFO__ = 'deviceConfig.json'
    __META_DATASETINFO__ = 'datasetInfo.json'
    
    __DATACSV_PATH__ = "01_data\\csv\\"
    __DATABIN_PATH__ = "01_data\\bin\\"
    __LABELS_PATH__ = "01_data\\labels\\"
    __DATAPLOT_PATH__ = "01_data\\plots\\"
    
    __BMA456_DATANAMES__ = ['timestamp[s]','bma456ax[lsb]','bma456ay[lsb]','bma456az[lsb]']
    __BMA400_DATANAMES__ = ['timestamp[s]','bma400ax[lsb]','bma400ay[lsb]','bma400az[lsb]']
    __BMI270_ACCEL_DATANAMES__ = ['timestamp[s]','bmi270ax[lsb]','bmi270ay[lsb]','bmi270az[lsb]']
    __BMI270_GYRO_DATANAMES__ = ['timestamp[s]','bmi270gx[lsb]','bmi270gy[lsb]','bmi270gz[lsb]']
    __BMI260_ACCEL_DATANAMES__ = ['timestamp[s]','bmi260ax[lsb]','bmi260ay[lsb]','bmi260az[lsb]']
    __BMI260_GYRO_DATANAMES__ = ['timestamp[s]','bmi260gx[lsb]','bmi260gy[lsb]','bmi260gz[lsb]']
    __BMI160_ACCEL_DATANAMES__ = ['timestamp[s]','bmi160ax[lsb]','bmi160ay[lsb]','bmi160az[lsb]']
    __BMI160_GYRO_DATANAMES__ = ['timestamp[s]','bmi160gx[lsb]','bmi160gy[lsb]','bmi160gz[lsb]']
    __BMI085_ACCEL_DATANAMES__ = ['timestamp[s]','bmi085ax[lsb]','bmi085ay[lsb]','bmi085az[lsb]']
    __BMI085_GYRO_DATANAMES__ = ['timestamp[s]','bmi085gx[lsb]','bmi085gy[lsb]','bmi085gz[lsb]']
    __BMI088_ACCEL_DATANAMES__ = ['timestamp[s]','bmi088ax[lsb]','bmi088ay[lsb]','bmi088az[lsb]']
    __BMI088_GYRO_DATANAMES__ = ['timestamp[s]','bmi088gx[lsb]','bmi088gy[lsb]','bmi088gz[lsb]']
    __BMM150_MAG_DATANAMES__ = ["timestamp[s]","bmm150mx[lsb]","bmm150my[lsb]","bmm150mz[lsb]"]
    __BMP390_PRES_DATANAMES__ = ["timestamp[s]","bmp390p[hPa]","bmp390t[degC]"]
    __BMP380_PRES_DATANAMES__ = ["timestamp[s]","bmp380p[hPa]","bmp380t[degC]"]
    __BMP580_PRES_DATANAMES__ = ["timestamp[s]","bmp580p[hPa]","bmp580t[degC]"]
    __TMG4903_PROX_DATANAMES__ = ["timestamp[s]","TMG_Prox[lsb]"]
    
    
    def get_version(self):
        """
            get version.
        """
        print("[INFO]: meta data handler version: %s"%(self.__version__))
        return self.__version__
    
    def __init__(self):
        """
            initialization of the class.
        """
        # meta
        self.datasetinfo = []
        self.fileinfo = []
        self.activityinfo = []
        self.userinfo = []
        self.deviceconfigs = []
        # data
        self.csvDataList = []
        self.binDataList = []
        self.labelDataList = []
        self.data_a = []
        self.labels_a = []
        self.labels_df = []
        self.TIME_DT = 1

    ###################### GENERIC FILES HANDLER ######################################################
    def get_binaries(self, directory):
        """
            returns binary files from dataset 01_data\\bin\ repository
        """
        bindir = directory + self.__DATABIN_PATH__
        binaries_list = []
        for root, dirs, files in os.walk(bindir):
            for file in files:            
                if file.endswith(".bin"):
                    fullpath = os.path.abspath(os.path.join(root, file))
                    # fullpath=os.path.join(root, file)
                    binaries_list.append(fullpath)
        return binaries_list
    
    def get_bindata_files(self, directory):
        """
            returns binary files from dataset 01_data\\bin\ repository
        """
        bindir = directory + self.__DATABIN_PATH__
        fileListOut = {"file_name":[],"file_path":[],"file_fullpath":[]}
        for root, dirs, files in os.walk(bindir):
            for file in files:            
                if file.endswith(".bin"):
                    fullpath = os.path.abspath(os.path.join(root, file))
                    fileListOut["file_name"].append(file)
                    fileListOut["file_path"].append(root)
                    fileListOut["file_fullpath"].append(fullpath)
        self.binDataList = fileListOut
        return fileListOut
    
    def get_csvdata_files(self, directory):
        """
            returns binary files from dataset 01_data\\bin\ repository
        """
        tmpdir = directory + self.__DATACSV_PATH__
        fileListOut = {"file_name":[],"file_path":[],"file_fullpath":[]}
        for root, dirs, files in os.walk(tmpdir):
            for file in files:            
                if file.endswith(".csv"):
                    fullpath = os.path.abspath(os.path.join(root, file))
                    fileListOut["file_name"].append(file)
                    fileListOut["file_path"].append(root)
                    fileListOut["file_fullpath"].append(fullpath)
        self.csvDataList = fileListOut
        return fileListOut
    
    def get_labels_files(self, directory):
        """
            returns binary files from dataset 01_data\\bin\ repository
        """
        tmpdir = directory + self.__LABELS_PATH__
        fileListOut = {"file_name":[],"file_path":[],"file_fullpath":[]}
        for root, dirs, files in os.walk(tmpdir):
            for file in files:            
                if file.endswith(".csv"):
                    fullpath = os.path.abspath(os.path.join(root, file))
                    fileListOut["file_name"].append(file)
                    fileListOut["file_path"].append(root)
                    fileListOut["file_fullpath"].append(fullpath)
        self.labelDataList = fileListOut
        return fileListOut
    
    def list_metafiles(self,datasetpath):
        """
            list the metafiles in the dataset folder
        """
        metadir = datasetpath+self.__META_PATH__
        print('[INFO]: meta-files in directory %s'%(os.path.abspath(metadir)))
        for root, dirs, files in os.walk(metadir):
            for file in files:            
                fullpath = os.path.abspath(os.path.join(root, file))
                print("\t"+fullpath)
    
    def list_datacsvfiles(self,datasetpath):
        """
            list the datafiles (csv) in the dataset data-folder
        """
        csvdir = datasetpath+self.__DATACSV_PATH__
        print('[INFO]: csv-data files in directory %s'%(os.path.abspath(csvdir)))
        for root, dirs, files in os.walk(csvdir):
            for file in files:
                if file.endswith(".csv"):
                    fullpath = os.path.abspath(os.path.join(root, file))
                    print("\t"+fullpath)
    
    def list_labelsfiles(self,datasetpath):
        """
            list the labels files in the dataset folder
        """
        labeldir = datasetpath+self.__LABELS_PATH__
        print('[INFO]: labels-files in directory %s'%(os.path.abspath(labeldir)))
        for root, dirs, files in os.walk(labeldir):
            for file in files:
                if file.endswith(".csv"):
                    fullpath = os.path.abspath(os.path.join(root, file))
                    print("\t"+fullpath)
                    
    ###################### CONVERSION ######################################
    def check_and_create_folder(self,datasetpath,directory):
        """
            checks if all folders are exists and creates the foilders if necessary
        """
        # check if not exists --> create    
        if not os.path.isdir(os.path.join(datasetpath,directory)):
            print('[INFO] dir: %s --> NOT exists'%(directory))
            try:
                os.makedirs(os.path.join(datasetpath,directory))
                if os.path.isdir(os.path.join(datasetpath,directory)):
                    print('[INFO]\tfolder created %s'%(directory))
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
                    
    def check_dataset_folder_structure(self,datasetpath):
        """
            checks and creates necessary folders
        """
        self.check_and_create_folder(datasetpath,self.__DATACSV_PATH__)
        self.check_and_create_folder(datasetpath,self.__DATABIN_PATH__)
        self.check_and_create_folder(datasetpath,self.__DATAPLOT_PATH__)
        self.check_and_create_folder(datasetpath,self.__LABELS_PATH__)
    
    def convert_bin2csv(self,datasetpath):
        """
            converts the all binary files in 01_data/bin to csv and stored in 01_data/csv
        """
        binary_list = self.get_bindata_files(datasetpath)
        self.check_dataset_folder_structure(datasetpath)
        # decode all binaries
        glpDecoder = GLP_Decoder.GLP_Decoder()
        #fileformat = "csv"
        #altname = None
        #    print(each)
        for each_binary in binary_list["file_fullpath"]:
            filename = each_binary
            save_filename = re.sub(r'\\bin\\', r'\\csv\\', filename)
            save_filename = save_filename.replace('.bin', '')
            save_png_filename = re.sub(r'bin\\', r'plots\\', filename)
            save_png_filename = save_png_filename.replace('.bin', '')
            print("[INFO]\t\tconverting file: " + filename)
            glpDecoder.reset()
            glpDecoder.read_bin(filename)
            glpDecoder.save_plot_data(save_png_filename,0)
            glpDecoder.make_file(save_filename, "csv", None)

    
    ###################### META DATA HANDLER FUNCTIONS  ################################################
    def get_fileinfo(self,datasetpath):
        """
            reads file info meta from meta
        """
        df=pd.read_csv(datasetpath + self.__META_PATH__ + self.__META_FILEINFO__)
        self.fileinfo = df
    
    def get_userinfo(self,datasetpath):
        """
            reads user info meta from meta
        """
        df=pd.read_csv(datasetpath + self.__META_PATH__ + self.__META_USERINFO__)
        self.userinfo = df
    
    def get_activityinfo(self,datasetpath):
        """
            reads activity info meta from meta
        """
        df=pd.read_csv(datasetpath + self.__META_PATH__ + self.__META_ACTIVITY__)
        self.activityinfo = df
    
    def extract_sensorconfigs_from_json(self,jsonobj):
        """
            extracts sensor configuration from kjson strings.
        """
        flag = False
        # "type":"SENSOR","dev_id":"FE:E6:72:B6:C4:C0","sensor_type":"ACCEL","name":"BMI270_ACCEL","range":"SENSOR_ACCEL_RANGE_8G","bw":"SENSOR_ACCEL_BW_NORMAL_AVG4","odr":"SENSOR_ODR_100HZ","performance_mode":"SENSOR_CONT_MODE","unit":"LSB","samplingfreq":"100"
        sensorconfig = {"dev_id":None,"sensor_type":None,"name":None,"range":None,"bw":None,"odr":None,"unit":None,"samplingfreq":None}        
        if jsonobj["sensor_type"] == 'ACCEL':
            if jsonobj["name"] == 'BMI055_ACCEL':
                sensorconfig["dev_id"] = jsonobj["dev_id"]
                sensorconfig["sensor_type"] = jsonobj["sensor_type"]
                sensorconfig["name"] = jsonobj["name"]
                sensorconfig["range"] = jsonobj["range"]
                sensorconfig["bw"] = jsonobj["bw"]
                sensorconfig["unit"] = jsonobj["unit"]
                sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
                flag = True
            else:
                sensorconfig["dev_id"] = jsonobj["dev_id"]
                sensorconfig["sensor_type"] = jsonobj["sensor_type"]
                sensorconfig["name"] = jsonobj["name"]
                sensorconfig["range"] = jsonobj["range"]
                sensorconfig["bw"] = jsonobj["bw"]
                sensorconfig["odr"] = jsonobj["odr"]
                sensorconfig["unit"] = jsonobj["unit"]
                sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
                flag = True              
        elif jsonobj["sensor_type"] == 'GYRO':
            if jsonobj["name"] == 'BMI085_GYRO' or jsonobj["name"] == 'BMI088_GYRO' or jsonobj["name"] == 'BMI055_GYRO':               
                sensorconfig["dev_id"] = jsonobj["dev_id"]
                sensorconfig["sensor_type"] = jsonobj["sensor_type"]
                sensorconfig["name"] = jsonobj["name"]
                sensorconfig["range"] = jsonobj["range"]
                sensorconfig["bw"] = jsonobj["bw"]
                sensorconfig["unit"] = jsonobj["unit"]
                sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
                flag = True
            else:
                sensorconfig["dev_id"] = jsonobj["dev_id"]
                sensorconfig["sensor_type"] = jsonobj["sensor_type"]
                sensorconfig["name"] = jsonobj["name"]
                sensorconfig["range"] = jsonobj["range"]
                sensorconfig["bw"] = jsonobj["bw"]
                sensorconfig["odr"] = jsonobj["odr"]
                sensorconfig["unit"] = jsonobj["unit"]
                sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
                flag = True                
        elif jsonobj["sensor_type"] == 'MAG':
            flag = True
            sensorconfig["dev_id"] = jsonobj["dev_id"]
            sensorconfig["sensor_type"] = jsonobj["sensor_type"]
            sensorconfig["name"] = jsonobj["name"]
            #sensorconfig["odr"] = jsonobj["odr"]
            sensorconfig["unit"] = jsonobj["unit"]
            sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
        elif jsonobj["sensor_type"] == 'PRESSURE':
            flag = True
            sensorconfig["dev_id"] = jsonobj["dev_id"]
            sensorconfig["sensor_type"] = jsonobj["sensor_type"]
            sensorconfig["name"] = jsonobj["name"]
            sensorconfig["odr"] = jsonobj["odr"]
            sensorconfig["unit"] = jsonobj["unit"]
            sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
        elif jsonobj["sensor_type"] == 'PROXIMITY':
            flag = True
            sensorconfig["dev_id"] = jsonobj["dev_id"]
            sensorconfig["sensor_type"] = jsonobj["sensor_type"]
            sensorconfig["name"] = jsonobj["name"]
            sensorconfig["odr"] = jsonobj["odr"]
            sensorconfig["unit"] = jsonobj["unit"]
            sensorconfig["samplingfreq"] = jsonobj["samplingfreq"]
            #print("[TODO]: pressure sensor configs need to be implemented.")
        #else:
        #    print("[ERROR]: unknown sensor type.")
        return sensorconfig
    
    def extract_filemeta_from_json(self,jsonobj):
        """
            reads file info meta from json
        """
        # {"type":"FILEMETA","file_id":"1579095291391","file_name":"LOG_CA4E50_1579095291389.bin"}
        flag=False
        out = {"file_id":None,"file_name":None}        
        if jsonobj["type"]=='FILEMETA':
            out["file_id"] = jsonobj["file_id"]
            out["file_name"] = jsonobj["file_name"]
            flag=True
        return out
    
    def extract_deviceconfig_from_json(self,jsonobj):
        """
            extracts device meta from json
        """
        # {"type":"FILEMETA","file_id":"1579095291391","file_name":"LOG_CA4E50_1579095291389.bin"}
        flag=False
        out = {"dev_id":None,"dev_loc":None,"dev_fw":None,"dev_name":None,"type":None}
        if jsonobj["type"] == 'DEVICE':
            out["dev_id"] = jsonobj["dev_id"]
            out["dev_loc"] = jsonobj["dev_loc"]
            out["dev_fw"] = jsonobj["dev_fw"]
            out["dev_name"] = jsonobj["dev_name"]
            out["type"] = jsonobj["type"]
            flag=True
        return out
    
    def get_datasetinfo(self,datasetpath):
        """
            reads dataset info and stores in datasetinfo
        """
        out = {"name":None,"description":None,"date_of_creation":None}        
        with open(datasetpath +self.__META_PATH__ + self.__META_DATASETINFO__,'r') as fp:
            line = fp.readline()            
            jsonobj = json.loads(line)
            try:
                out["name"] = jsonobj["name"]
                out["description"] = jsonobj["description"]
                out["date_of_creation"] = jsonobj["date_of_creation"]
                self.datasetinfo = out
            except:
                print("[ERROR] wrong json format for dataset meta-info.")
        return out

    def get_device_configs(self, datasetpath):
        """
        extracts the sensor configurations from the meta data.
        
        Returns:
            - device configs ()
            - number of sensors
            - 
        """
        with open(datasetpath +self.__META_PATH__ + self.__META_DEVICEINFO__,'r') as fp:
            line = fp.readline()            
            while line:
                devCFG = {"file_name":[], "file_id":[],"dev_id":[],"dev_loc":[],"dev_fw":[],"dev_name":[],"type":[],"sensors":[]}
                file_meta = None
                device_cfg = None
                jsonobj = json.loads(line)
                              
                for idx in range(len(jsonobj)):
                    if jsonobj[idx]["type"]=='FILEMETA':
                        file_meta = self.extract_filemeta_from_json(jsonobj[idx])
                        devCFG["file_name"]=file_meta["file_name"]
                        devCFG["file_id"]=file_meta["file_id"]
                        #print("-----file----meta--------")
                        #display(file_meta)
                    if jsonobj[idx]["type"]=='DEVICE':
                        device_cfg = self.extract_deviceconfig_from_json(jsonobj[idx])
                        devCFG["dev_id"]=device_cfg["dev_id"]
                        devCFG["dev_loc"]=device_cfg["dev_loc"]
                        devCFG["dev_fw"]=device_cfg["dev_fw"]
                        devCFG["dev_name"]=device_cfg["dev_name"]
                        devCFG["type"]=device_cfg["type"]
                        #print("-----device----cfg--------")
                        #display(device_cfg)
                    if jsonobj[idx]["type"]=='SENSOR':
                        sensor_cfg = self.extract_sensorconfigs_from_json(jsonobj[idx])
                        devCFG["sensors"].append(sensor_cfg)
                        #print("-----sensor----cfg--------")
                        #display(sensor_cfg)
                line = fp.readline()
                self.deviceconfigs.append(devCFG)
    
    def config_get_sensors(self,filename):
        """
            reads sensor configurations for filename (binary)
        """
        out = None
        for devCFG in self.deviceconfigs:
            if devCFG["file_name"] == filename:
                out = devCFG["sensors"]
        return out
    
    def is_sensor_logged(self,filename,sensor_type):
        """
            reads file info meta from meta
        """
        out = False
        sensors = self.config_get_sensors(filename)
        for sensor in sensors:
            if sensor["sensor_type"]==sensor_type:
                out = True
        return out
    
    def get_sensor_configs(self,sensors_cfg):
        """
            returns sensor configs accroding to range, bits etc...
        """
        out = {"sensor_type":[],
               "name":[],
               "range":[],
               "odr":[],
               "bw":[],
               "lsb2unit":[]}
        if sensors_cfg["sensor_type"]=="ACCEL":
            out["sensor_type"] = sensors_cfg["sensor_type"]
            out["name"] = sensors_cfg["name"]
            out["bw"] = sensors_cfg["bw"]
            out["odr"] = sensors_cfg["odr"]
            if sensors_cfg["range"] == 'SENSOR_ACCEL_RANGE_2G':
                out["range"] = 2
                out["lsb2unit"] = out["range"]/(2**15-1)
            elif sensors_cfg["range"] == 'SENSOR_ACCEL_RANGE_4G':
                out["range"] = 4
                out["lsb2unit"] = out["range"]/(2**15-1)
            elif sensors_cfg["range"] == 'SENSOR_ACCEL_RANGE_8G':
                out["range"] = 8
                out["lsb2unit"] = out["range"]/(2**15-1)
            elif sensors_cfg["range"] == 'SENSOR_ACCEL_RANGE_16G':
                out["range"] = 16
                out["lsb2unit"] = out["range"]/(2**15-1)
            else:
                print("[ERROR]: unknown accel range.")
        elif sensors_cfg["sensor_type"]=="GYRO":
            out["sensor_type"] = sensors_cfg["sensor_type"]
            out["name"] = sensors_cfg["name"]
            out["bw"] = sensors_cfg["bw"]
            out["odr"] = sensors_cfg["odr"]
            if sensors_cfg["range"] == 'SENSOR_GYRO_RANGE_2000_DPS':
                out["range"] = 2000
                out["lsb2unit"] = out["range"]/(2**15-1)
            elif sensors_cfg["range"] == 'SENSOR_GYRO_RANGE_1000_DPS':
                out["range"] = 1000
                out["lsb2unit"] = out["range"]/(2**15-1)
            elif sensors_cfg["range"] == 'SENSOR_GYRO_RANGE_500_DPS':
                out["range"] = 500
                out["lsb2unit"] = out["range"]/(2**15-1)
            elif sensors_cfg["range"] == 'SENSOR_GYRO_RANGE_250_DPS':
                out["range"] = 250
                out["lsb2unit"] = out["range"]/(2**15-1)
            else:
                print("[ERROR]: unknown gyro range.")
        elif sensors_cfg["sensor_type"]=="MAG":
            out["sensor_type"] = sensors_cfg["sensor_type"]
            out["name"] = sensors_cfg["name"]
            #out["bw"] = sensors_cfg["bw"]
            out["odr"] = 20
            out["range"] = 1300
            out["lsb2unit"] = 1/16
        elif sensors_cfg["sensor_type"]=="PRESSURE":
            out["sensor_type"] = sensors_cfg["sensor_type"]
            out["name"] = sensors_cfg["name"]
            #out["bw"] = sensors_cfg["bw"]
            out["odr"] = sensors_cfg["odr"]
            out["range"] = 1300
            out["lsb2unit"] = 1
        elif sensors_cfg["sensor_type"]=="PROXIMITY":
            out["sensor_type"] = sensors_cfg["sensor_type"]
            out["name"] = sensors_cfg["name"]
            #out["bw"] = sensors_cfg["bw"]
            out["odr"] = sensors_cfg["odr"]
            out["range"] = 16576
            out["lsb2unit"] = 1
        else:
            print("[ERROR]: unknown sensor type.")
        return out            

    # ------------- DATA HANDLER FUNCTIONS ---------------- #
    def read_csvfile(self, file):
        """
            reads file and returns data as dataframe
        """
        df=pd.read_csv(file)
        return df
    
    def get_sensordata(self, dataf, sensor_name=None):
        """
            gets sensor data from dataframe for predefined sensor name (e.g. BMM150)
        """
        names = None
        if sensor_name == "BMA456":
                names = self.__BMA456_DATANAMES__
        elif sensor_name == "BMI270_ACCEL":
                names = self.__BMI270_ACCEL_DATANAMES__
        elif sensor_name == "BMI270_GYRO":
                names = self.__BMI270_GYRO_DATANAMES__
        elif sensor_name == "BMM150":
                names = self.__BMM150_MAG_DATANAMES__
        elif sensor_name == "BMP390":
                names = self.__BMP390_PRES_DATANAMES__
        elif sensor_name == "TMG4903":
                names = self.__TMG4903_PROX_DATANAMES__
        elif sensor_name == "BMP380":
                names = self.__BMP380_PRES_DATANAMES__
        elif sensor_name == "BMP580":
                names = self.__BMP580_PRES_DATANAMES__
        elif sensor_name == "BMI160_ACCEL":
                names = self.__BMI160_ACCEL_DATANAMES__
        elif sensor_name == "BMI160_GYRO":
                names = self.__BMI160_GYRO_DATANAMES__
        elif sensor_name == "BMI260_ACCEL":
                names = self.__BMI260_ACCEL_DATANAMES__
        elif sensor_name == "BMI260_GYRO":
                names = self.__BMI260_GYRO_DATANAMES__
        elif sensor_name == "BMI085_ACCEL":
                names = self.__BMI085_ACCEL_DATANAMES__
        elif sensor_name == "BMI085_GYRO":
                names = self.__BMI085_GYRO_DATANAMES__
        elif sensor_name == "BMI088_ACCEL":
                names = self.__BMI088_ACCEL_DATANAMES__
        elif sensor_name == "BMI088_GYRO":
                names = self.__BMI088_GYRO_DATANAMES__
        elif sensor_name == "BMA400":
                names = self.__BMA400_DATANAMES__
        else:
            print("[ERROR]: unknown sensor.")
                
        if names is not None:            
            data = dataf[names]
        else:
            data = None
        return data
        
    def plot_data(self, dataf, sensorType, dt, sf, title=''):
        """
            plots the data according to sensor type / name (e.g. BMA456)
        """
        if sensorType == "BMA456":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bma456ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bma456ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bma456az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMI270_ACCEL":
            tVec = dataf["timestamp[s]"]*dt  
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi270ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bmi270ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bmi270az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        
        elif sensorType == "BMI270_GYRO":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi270gx[lsb]"]*sf,label="wx")
            plt.plot(tVec,dataf["bmi270gy[lsb]"]*sf,label="wy")
            plt.plot(tVec,dataf["bmi270gz[lsb]"]*sf,label="wz")
            plt.xlabel("time (s)")
            plt.ylabel("angular rate (dps)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMI260_ACCEL":
            tVec = dataf["timestamp[s]"]*dt  
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi260ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bmi260ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bmi260az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        
        elif sensorType == "BMI260_GYRO":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi260gx[lsb]"]*sf,label="gx")
            plt.plot(tVec,dataf["bmi260gy[lsb]"]*sf,label="gy")
            plt.plot(tVec,dataf["bmi260gz[lsb]"]*sf,label="gz")
            plt.xlabel("time (s)")
            plt.ylabel("angular rate (dps)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMI160ACCEL":
            tVec = dataf["timestamp[s]"]*dt  
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi160ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bmi160ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bmi160az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        
        elif sensorType == "BMI160GYRO":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi160gx[lsb]"]*sf,label="gx")
            plt.plot(tVec,dataf["bmi160gy[lsb]"]*sf,label="gy")
            plt.plot(tVec,dataf["bmi160gz[lsb]"]*sf,label="gz")
            plt.xlabel("time (s)")
            plt.ylabel("angular rate (dps)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMI085_ACCEL":
            tVec = dataf["timestamp[s]"]*dt  
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi085ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bmi085ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bmi085az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        
        elif sensorType == "BMI085_GYRO":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi085gx[lsb]"]*sf,label="gx")
            plt.plot(tVec,dataf["bmi085gy[lsb]"]*sf,label="gy")
            plt.plot(tVec,dataf["bmi085gz[lsb]"]*sf,label="gz")
            plt.xlabel("time (s)")
            plt.ylabel("angular rate (dps)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMI088_ACCEL":
            tVec = dataf["timestamp[s]"]*dt  
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi088ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bmi088ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bmi088az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        
        elif sensorType == "BMI088_GYRO":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmi088gx[lsb]"]*sf,label="gx")
            plt.plot(tVec,dataf["bmi088gy[lsb]"]*sf,label="gy")
            plt.plot(tVec,dataf["bmi088gz[lsb]"]*sf,label="gz")
            plt.xlabel("time (s)")
            plt.ylabel("angular rate (dps)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMM150":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmm150mx[lsb]"]*sf,label="mx")
            plt.plot(tVec,dataf["bmm150my[lsb]"]*sf,label="my")
            plt.plot(tVec,dataf["bmm150mz[lsb]"]*sf,label="mz")
            plt.xlabel("time (s)")
            plt.ylabel("magnetic field (uT)")
            plt.grid()
            plt.legend()
        
        elif sensorType == "BMP390":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmp390p[hPa]"],label="pres")
            plt.xlabel("time (s)")
            plt.ylabel("pressure (hPa)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMP380":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmp380p[hPa]"],label="pres")
            plt.xlabel("time (s)")
            plt.ylabel("pressure (hPa)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMP580":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bmp580p[hPa]"],label="pres")
            plt.xlabel("time (s)")
            plt.ylabel("pressure (hPa)")
            plt.grid()
            plt.legend()
        elif sensorType == "BMA400":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["bma400ax[lsb]"]*sf,label="ax")
            plt.plot(tVec,dataf["bma400ay[lsb]"]*sf,label="ay")
            plt.plot(tVec,dataf["bma400az[lsb]"]*sf,label="az")
            plt.xlabel("time (s)")
            plt.ylabel("aceleration (g)")
            plt.grid()
            plt.legend()
        elif sensorType == "TMG4903":
            tVec = dataf["timestamp[s]"]*dt
            plt.title(sensorType+" data "+title)
            plt.plot(tVec,dataf["TMG_Prox[lsb]"],label="prox")
            plt.xlabel("time (s)")
            plt.ylabel("proximity (lsb)")
            plt.grid()
            plt.legend()
        else:
            print("[ERROR]: unknown sensor type for plotting")
    
    ########################################################################
    def read_labelsfile(self,file):
        """
            reads labels file
        """
        if os.path.isfile(file):
            out = pd.read_csv(file)
            self.labeldf = out
        else:
            out = None
        return out
    
    def get_labelnames(self,label_df):
        """
            extracts the label names and id from the label_df
        """
        label_tags = {"label_id":[],"label_name":[]}        
        for idx in range(len(label_df["label_id"])):
            if label_df["label_id"][idx] not in label_tags["label_id"]:
                label_tags["label_id"].append(label_df["label_id"][idx])
                label_tags["label_name"].append(label_df["label_name"][idx])
        return label_tags
    
    def extract_labels_with_sameid(self,label_df,label_id):
        """
            returns all data with the same label id
        """
        out_labels = {"start_time":[],"stop_time":[],"label_id":[],"label_name":[],"device_id":[]}
        idx_ = (label_df["label_id"] == label_id)
        out_labels = label_df[:].loc[idx_]
        out_labels = out_labels.reset_index(drop=True)
        return out_labels
    
    def extract_labels_with_samename(self,label_df,label_name):
        """
            returns all data with the same label name
        """
        out_labels = {"start_time":[],"stop_time":[],"label_id":[],"label_name":[],"device_id":[]}
        idx_ = (label_df["label_name"] == label_name)
        out_labels = label_df[:].loc[idx_]
        out_labels = out_labels.reset_index(drop=True)
        return out_labels
    
    def create_labelsignals(self,t_vec,t_start,labels):
        """
            creates labeled time series according to labels
        """
        L = len(t_vec)
        tmp_id = np.zeros_like(t_vec)
        label_out = {"time":t_vec,"label_id":tmp_id,"label_name":['UNKNOWN']*L}
        
        for ii in range(len(labels["label_id"])):
            dt1 = datetime.datetime.strptime(labels["start_time"][ii], '%Y-%m-%d %H:%M:%S.%f') - datetime.datetime.strptime(t_start, '%Y-%m-%d %H:%M:%S.%f')
            dt2 = datetime.datetime.strptime(labels["stop_time"][ii], '%Y-%m-%d %H:%M:%S.%f') - datetime.datetime.strptime(t_start, '%Y-%m-%d %H:%M:%S.%f')
            indx_ = (t_vec>=dt1.total_seconds()) == (t_vec<dt2.total_seconds())
            label_out["label_id"][indx_] = labels["label_id"][ii]
            #label_out["label_name",indx_] = labels["label_name"][ii]
        
        return label_out
    
    def check_dataset(self,datasetDir,verbose=True):
        """
            do check the meta info and files logged in the dataset.
        """
        # read the meta files
        self.get_fileinfo(datasetDir)
        self.get_activityinfo(datasetDir)
        self.get_userinfo(datasetDir)
        self.get_device_configs(datasetDir)
        
        # 3. data files and label files from the repository
        self.get_csvdata_files(datasetDir)
        self.get_bindata_files(datasetDir)
        self.get_labels_files(datasetDir)
        
        num_of_total_files = 0        
        num_of_missing_files = 0
        print("[INFO]: check dataset repo: %s"%(datasetDir))
        for idx in range(len(self.fileinfo["file_name"])):
            filename = self.fileinfo["file_name"][idx]
            num_of_total_files +=1
            if filename in self.binDataList["file_name"]:
                if verbose:
                    print("[INFO] file %s found."%(filename))
                    print("\tstart: \t%s"%(self.fileinfo["start_time"][idx]))
                    print("\tstop: \t%s"%(self.fileinfo["stop_time"][idx]))
                
            else:
                num_of_missing_files +=1
                if verbose:
                    print("[WARNING] file %s not found. "%(filename))
        print("[INFO]: files in repo: %i"%(num_of_total_files))
        print("[INFO]: missing files: %i"%(num_of_missing_files))

        

    def read_dataset(self,datasetDir):
        """
            do read the dataset data, sensor settings and labels.
            
            1. read data files in dataset
            2. check if file is labeled
            3. reads the labels
            4. creates the labels sequence
        """
        
        self.get_fileinfo(datasetDir)
        self.get_activityinfo(datasetDir)
        self.get_userinfo(datasetDir)
        self.get_device_configs(datasetDir)
    
        # 3. data files and label files from the repository
        self.get_csvdata_files(datasetDir)
        self.get_bindata_files(datasetDir)
        self.get_labels_files(datasetDir)
    
        # 4. do process the files and plot data and labels\n",
        for fileName in self.csvDataList["file_name"]:
            print("[INFO] reading file: %s"%(fileName))            
            
            # get index of the file to be processed
            file_idx = self.csvDataList["file_name"].index(fileName)
                                    
            # read data from csv file
            data_df = self.read_csvfile(self.csvDataList["file_fullpath"][file_idx])
            self.data_a.append(data_df)
            
            # do label extraction TODO
            tmp_labelFileName = 'LABEL_' + fileName
            
            if tmp_labelFileName in self.labelDataList["file_name"]:
                t_start = self.fileinfo["start_time"][file_idx]
                t_vec = data_df["timestamp[s]"]*self.TIME_DT
                labelFilePath = datasetDir + self.__LABELS_PATH__ + tmp_labelFileName
                label_df = pd.read_csv(labelFilePath)
                # extract labels with same id
                label_sig = self.create_labelsignals(t_vec,t_start,label_df)
                self.labels_df.append(label_df)
                self.labels_a.append(label_sig)
            else:
                self.labels_a.append(None)
                self.labels_df.append(None)
        

# ------------------- TEST FUNCTIONS ----------------- #
def main_check_datasets():
    """
        checks the results from the test dataset / tutorial
    """
    datasetList = []    
    datasetList.append("bmi270testdataset/")
    
    # evaluate repositories and extract data, labels and visualize them
    for datasetDir in datasetList:
        print("\\n[INFO] dataset repo \\t %s"%(os.path.abspath(datasetDir)))
        dataHandler = DatasetFormatHandler()
        dataHandler.check_dataset(datasetDir)

def main_plot_datasetdata():
    """
        plots the results from the test dataset / tutorial
    """
    datasetList = []
    datasetList.append("tutorial/")
    
    plt.rcParams['figure.figsize'] = (12, 10) # plot size parameters\n"
    
    for datasetDir in datasetList:
        print("\n[INFO] dataset repo \t %s"%(os.path.abspath(datasetDir)))
        dataHandler = DatasetFormatHandler()
        dataHandler.read_dataset(datasetDir)
        
        # 4. do process the files and plot data and labels\n",
        for fileName in dataHandler.csvDataList["file_fullpath"]:
            print("\\n[INFO] processing file: %s"%(fileName))
            
            # get index of the file to be processed
            file_idx = dataHandler.csvDataList["file_fullpath"].index(fileName)
            
            # read sensor configs for the logged file
            sensors_cfg = dataHandler.config_get_sensors(dataHandler.deviceconfigs[file_idx]["file_name"])
    
            # plot sensor data
            plt.figure()
            plot_idxL = len(sensors_cfg) + 1
            plot_idx = 1
            
            for sensor in sensors_cfg:
                # get sensor data
                sensordata = dataHandler.get_sensordata(dataHandler.data_a[file_idx],sensor["name"])
                # get sensor configurations
                senscfg = dataHandler.get_sensor_configs(sensor)
                # plot data
                plt.subplot(plot_idxL,1,plot_idx)
                dataHandler.plot_data(sensordata,senscfg["name"], dataHandler.TIME_DT, senscfg["lsb2unit"], title=dataHandler.csvDataList["file_name"][file_idx])
                plot_idx += 1
            
            plt.subplot(plot_idxL,1,plot_idx)
            if dataHandler.labels_a[file_idx] is not None:
                plt.title('LABELS '+dataHandler.csvDataList["file_name"][file_idx])
                plt.plot(dataHandler.labels_a[file_idx]["time"],dataHandler.labels_a[file_idx]["label_id"],label='label_id')
                plt.xlabel("time (s)")
                plt.ylabel("label id")
            plt.tight_layout()
        
        plt.show()  
    
###############################################################################################
###############################################################################################
if __name__ == "__main__":
    main_check_datasets()    
    main_plot_datasetdata()
