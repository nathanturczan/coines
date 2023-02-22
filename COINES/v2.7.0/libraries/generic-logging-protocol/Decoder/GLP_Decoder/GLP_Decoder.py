# -*- coding: utf-8 -*-
"""
Created on Fri Apr 20 12:12:35 2018
Decodes Binary Files in GLP to CSV or JSON.

GLP header:
                                            times number of acitve channels
                                  ______________________|______________________
                                 /                                             \
                                |                                               |
header  | protocol  | number of | channel   | channel   | channel   | channel   | desc.     |descr.     | header
start   | version   | active ch.| id        | abb.      | format    | unit      | size      |           | stop
--------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------
0xCCCC  | 1 Byte    | 1 Byte    | 1 Byte    | 8 Byte    | 1 Byte    | 1 Byte    | 2 Byte    |desc. Size | 0x3333    
        |           |           |           | name      | f,i,c...  | raw,d/s...|           |Byte       |          
        
        
GLP frame:

frame   | frame     | timestamp | packet    | data rdy  | data rdy  | channel_0 | ...       | channel_N | crc32     | frame 
start   | length    |           | count     | count     | Bits      | payload   |           | payload   |           | stop
--------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|----------
0x9999  | 2 Byte    | 8 Byte    | 1 Byte    | 1 Byte    | 4 Byte    | according | ...       |according  |           | 0x6666    
        |           |           |           | name      |           | to format |           |to format  |           |          
    
Example for data rdy Bits:  32          24          16          8
                            00000000    00000000    00001000    10000011        --->        Data rdy on channel 1,2,8 and 12
    
@author: FRY7RT
"""
import struct
import csv, json
import os
import numpy as np
from datetime import datetime
#from pyreadline.lineeditor.lineobj import quote_char
from _csv import QUOTE_MINIMAL
import sys
import matplotlib.pyplot as plt
import argparse

GLP_HEADER_START = 0xCC
GLP_HEADER_STOP = 0x33
GLP_FRAME_START = 0x99
GLP_FRAME_STOP = 0x66

GLP_NUM_OF_CHANNELS_MAX = 32    # maximum number of channels

################################################
class GLP_Decoder(object):
    def __init__(self):
        self.channel_cnt = 0                        #number of channels, used in total
        self.glp_header = {"ch_name":[None] * GLP_NUM_OF_CHANNELS_MAX, "ch_format":[None] * GLP_NUM_OF_CHANNELS_MAX, "ch_unit":[None] * GLP_NUM_OF_CHANNELS_MAX, "ch_format_size" : [None] * GLP_NUM_OF_CHANNELS_MAX, "ch_active" : [0] * GLP_NUM_OF_CHANNELS_MAX} #Container for header info and conversion info
        self.general_desc = ["", "time of conversion: ", "", "file: ", ""]
        self.data_rows = None                       #numpy array contains measurement data
        self.data_row_buf = None                    # raw buffer which contains the 
        self.channels_used = []                     #container for indexes of channels, declared in the header
        self.desc = ""                              #contains Description
        self.version = 0                            #GLP version
        self.plotDataFlag = False
        self.smallestTypeSize = 10                  #size of smallest used channel type, used for preallocation
    def reset(self):                                #reset file dependent variables
        self.channel_cnt =0 
        self.channels_used = []                 
        self.data_rows = None
        self.data_row_buf = None                    # raw buffer which contains the
        self.smallestTypeSize = 10
    def read_bin(self, filName):
        # read file as binarydata
        try:
            filObj = open(filName.encode('ascii'), 'rb')
            filCon = filObj.read()
            filObj.close()
            self.general_desc[2] = datetime.utcnow().isoformat()        #add information about conversion to description
            self.general_desc[4] = filName
        except Exception as e:
            print("problems reading file")
            print(e)
            return
        # get file size for read loop
        statinfo = os.stat(filName)
        channel_selector = [None] * len(self.glp_header["ch_active"])   #container for bitwise selection using & operator -->better performance
        for i in range(len(self.glp_header["ch_active"])):              #bitshifts for channel selection
            channel_selector[i] = (1 << i)
        datacount = 0                                                   #set counters to zero
        self.rowcnt = 0
        channels_active_bytes = 0
        # run through file looking for header/frame start
        while datacount < statinfo.st_size:
            try:
                if datacount >= statinfo.st_size - 4:                   #new frame to small to reach framelength --> eof reached
                    break
    #################################### process header #####################################
                if filCon[datacount] == GLP_HEADER_START:               #check for header start bytes 
                    datacount += 1;
                    if filCon[datacount] == GLP_HEADER_START:   
                        datacount += 1;
                        self.version = filCon[datacount]                ##add version specific changes here
                        if self.version < 2:                            #v1 has smaller identifier
                            identifier_length = 8
                        else:
                            identifier_length = 10
                        datacount += 1
                        self.channel_cnt = filCon[datacount]            #save channel count
                        datacount += 1;
                        for i in range(self.channel_cnt):               #collect channel information
                            ch_idx = filCon[datacount]                  #get the channel indes
                            self.channels_used.append(ch_idx)           #safe the channel index to the list of used indexes
                            self.glp_header["ch_name"][ch_idx] = "".join(map(chr, filCon[datacount + 1:datacount + identifier_length])) #get channel name
                            self.glp_header["ch_name"][ch_idx] = self.glp_header["ch_name"][ch_idx].strip()                 #Strip Spaces at the stringend
                            self.glp_header["ch_format"][ch_idx] = chr(filCon[datacount + identifier_length + 1])           #get the channel format
                            self.glp_header["ch_format_size"][ch_idx] = self.size_from_type(self.glp_header["ch_format"][ch_idx]) #calculate format size for channel
                            if filCon[datacount + identifier_length + 2] != 0:
                                self.glp_header["ch_unit"][ch_idx] = self.unit_from_identifier(chr(filCon[datacount + identifier_length + 2]))
                            datacount += identifier_length + 3
                        # check for version of protocol --> changed number of bytes for description --> more than 256 possible
                        if self.version > 1:
                            desc_size = (struct.unpack('H', bytes([filCon[datacount], filCon[datacount+1]])))[0]
                            datacount += 2
                        else:
                            desc_size = filCon[datacount] 
                            datacount += 1
                        self.desc = ""
                        for i in range(desc_size):                      #read description according to size
                            self.desc += chr(filCon[datacount + i])
                        self.desc = str(self.desc)
                        datacount += desc_size
                        #print('desciption: \t' + self.desc)
                        if filCon[datacount] != GLP_HEADER_STOP or filCon[datacount + 1] != GLP_HEADER_STOP:  ##check if header end is correct
                            print("FILEFORMAT_ERROR")
                        datacount += 2
                        for idx in self.channels_used:    #iterate through used channels
                            size = self.size_from_type(self.glp_header["ch_format"][idx]) #check for smallest file-type size for preallocation
                            if size < self.smallestTypeSize:            
                                self.smallestTypeSize = size
                        self.data_rows = np.zeros((round(statinfo.st_size / self.smallestTypeSize), self.channel_cnt + 2)) #preallocation for measurement data
                        self.data_row_buf = np.zeros((1,self.channel_cnt+2)) 
    #################################### process frame  #####################################                    
                elif filCon[datacount] == GLP_FRAME_START:                                          #check for frame start bytes
                    datacount += 1
                    if filCon[datacount] == GLP_FRAME_START:
                        datacount += 1
                        startcount = datacount                                                      #remember frame start for later length check
                        frame_length = filCon[datacount + 1] << 8 | filCon[datacount]               #get the 2 bytes frame length
                        if datacount + frame_length + 2 > statinfo.st_size:                         #framelength bigger than remaining filesize --> eof
                            break
                        datacount += 2
                        timestamp = filCon[datacount + 7] << 56 | filCon[datacount+6] << 48 | filCon[datacount + 5] << 40 | filCon[datacount + 4] << 32 |filCon[datacount + 3] << 24 | filCon[datacount+2] << 16 | filCon[datacount + 1] << 8 | filCon[datacount] #get the timestamp
                        self.data_rows[self.rowcnt][0] = timestamp * 0.000000001                       #calculate seconds from nanoseconds
                        self.data_row_buf[0,0] = self.data_rows[self.rowcnt][0]
                        datacount += 8                
                        packetcount = filCon[datacount]                                             #get the packetcount byte 
                        self.data_rows[self.rowcnt][1] = packetcount
                        self.data_row_buf[0,1] = self.data_rows[self.rowcnt][1]
                        datacount += 1
                        data_rdy_cnt = filCon[datacount]                                            #count of updated channels, currently unused
                        datacount += 1
                        channels_active_bytes = (filCon[datacount + 3] << 24 | filCon[datacount + 2] << 16 | filCon[datacount + 1] << 8 | filCon[datacount]) #collect active channel bytes to one variable
                        for i in self.channels_used:
                            self.glp_header["ch_active"][i] = (channels_active_bytes & channel_selector[i]) #check which channels are used and change in list
                        datacount += 4
                        for idx in self.channels_used:   ##collect data according to given type identifier
                            if self.glp_header['ch_active'][idx] != 0:                              #just take data from channel if its active
                                bytearr = filCon[datacount:datacount + self.glp_header["ch_format_size"][idx]]  #built array according to format size
                                self.data_rows[self.rowcnt][idx + 2] = (struct.unpack(self.glp_header["ch_format"][idx], bytes(bytearr)))[0] #fill measurement array
                                datacount += self.glp_header["ch_format_size"][idx]
                                self.data_row_buf[0,idx+2] = self.data_rows[self.rowcnt][idx+2]
                            #else:                                                                   #if channel is not active, try filling up with an old value or None
                            #    if self.rowcnt == 0:                                                #if first row and channel not active write None
                            #        self.data_rows[self.rowcnt][idx + 2] = None
                            #    else:                                                               #else use old data
                            #        self.data_rows[self.rowcnt][idx + 2] = self.data_rows[self.rowcnt - 1][idx + 2]
                        self.data_rows[self.rowcnt][:] = self.data_row_buf[0,:]
                        if filCon[datacount] != GLP_FRAME_STOP or filCon[datacount + 1] != GLP_FRAME_STOP or startcount + frame_length + 2 != datacount: #check if the stop bytes are set and the frame lenght is correct
                            print("FILEFORMAT_ERROR") 
                        self.rowcnt += 1
                        datacount += 2
                else:
                    datacount += 1                                                                  #if no frame or header not detected, just increase datacount
            except Exception as e:
                print('Error on line {}'.format(sys.exc_info()[-1].tb_lineno), type(e).__name__, e)
                print(e)
                print("datacount:")
                print(datacount)
                datacount += 1
        self.data_rows = self.data_rows[0:self.rowcnt]
        
#################################### data to file   #####################################
    def make_file(self, filName, form_out, altname = None):
        try:
            if altname is not None:
                newfile = altname
            else:
                newfile = str(filName)
            if '.' in newfile:
                newfile = newfile[:newfile.index('.')] + "." + form_out 
            else: 
                newfile = newfile + "." + form_out
                
            if form_out == "csv":
                header = ["timestamp[s]", "packetcount"]
                for idx in range(self.channel_cnt):
                    header += [str(self.glp_header["ch_name"][idx]) + "[" + str(self.glp_header["ch_unit"][idx]) + "]"]
                if '.' in newfile:
                    newfile = newfile[:newfile.index('.')] + ".csv" 
                else: 
                    newfile = newfile + ".csv"
                csvfile = open(newfile, 'w', newline = '')
                writer = csv.writer(csvfile, delimiter = ',')
                #print(self.general_desc[0].__class__.__name__)
                #writer.writerow([self.general_desc[0] + self.general_desc[1] + self.general_desc[2]]) # time of convesrion
                #writer.writerow([self.general_desc[3] + self.general_desc[4]]) # file name
                #writer.writerow(["GLP version: " + str(self.version)])
                #writer.writerow(['{\'description\':[' + self.desc + ']}'])
                #print(self.desc.__class__.__name__)
                writer.writerow(header)
                for row in self.data_rows:
                    writer.writerow(row)
                csvfile.close()
            if form_out == "json":
                jsonobj = json.loads('{"time of conversion": "", "file": "", "desc": "test", "channels_active" : {}, "data": []}')
                #jsonobj["time of conversion"] = self.general_desc[2]
                #jsonobj["GLP version"] = self.version
                jsonobj["original file"] = self.general_desc[4]
                jsonobj["desc"] = self.desc
                jsonhead = json.loads('{}')
                for idx in range(self.channel_cnt):
                    jsonhead[self.glp_header["ch_name"][idx]] = self.glp_header["ch_unit"][idx]
                jsonobj["channels_active"] = jsonhead
                for row in self.data_rows:
                    jsonrow = json.loads('{}')
                    jsonrow["timestamp[s]"] = row[0]
                    jsonrow["packetcount"] = row[1]
                    for idx in range(self.channel_cnt):
                        jsonrow[self.glp_header["ch_name"][idx]] = row[idx + 2]
                    jsonobj["data"].append(jsonrow)
                jsonfile = open(newfile, 'w', newline = '')
                jsonfile.write(json.dumps(jsonobj, sort_keys=False, indent=2, separators=(',', ': ')))
                jsonfile.close()
            #print("file saved")
        except Exception as e:
            print("file could not be saved")
            print(e)

#################################### plot the data while file gets processed ############
    def plot_data(self, show_packet_count):
        try:
            colors = ['r-', 'g-', 'b-', 'k-', 'm-', 'y-', 'c-']
            fig = plt.figure(self.desc, figsize=[12, 6])
            indx = 0
            if show_packet_count:
                show_packet_count = 1
            else:
                show_packet_count = 0
            print(self.channel_cnt)
            for indx in range(2, self.channel_cnt + 2):         #create a plot for every channel  
                if indx == 2:                                   #necessary for shared x axis
                    ax1 = plt.subplot((int)((self.channel_cnt + show_packet_count + 1) / 2), 2, indx - 1)
                    #plt.xlabel("time")
                else:
                    plt.subplot((int)((self.channel_cnt + show_packet_count + 1) / 2), 2, indx - 1, sharex = ax1)
                plt.ylabel(self.glp_header["ch_name"][indx - 2])
                plt.plot(self.data_rows[:, 0], self.data_rows[:, indx], colors[(indx - 2) % 7])
            if show_packet_count == 1:
                plt.subplot((int)((self.channel_cnt + show_packet_count + 1) / 2), 2, indx, sharex = ax1)
                plt.ylabel("packetcount",)
                plt.plot(self.data_rows[:, 0], self.data_rows[:, 1])
            plt.xlabel("time (s)")
            plt.tight_layout()                                  #reduce whitespace between panels to a minimum
            plt.show()
        except Exception as e:
            print('Error on line {}'.format(sys.exc_info()[-1].tb_lineno), type(e).__name__, e)

    def save_plot_data(self, filename, show_packet_count, show_plots=False):
        colors = ['r-', 'g-', 'b-', 'k-', 'm-', 'y-', 'c-']
        fig = plt.figure(self.desc, figsize=[16 * 2, 9 * 2])
        self.data_rows = np.array(self.data_rows)
        indx = 0
        if show_packet_count:
            show_packet_count = 1
        else:
            show_packet_count = 0
        for indx in range(2, self.channel_cnt + 2):             #create a plot for every channel 
            if indx == 2:                                       #necessary for shared x axis
                ax1 = plt.subplot((int)((self.channel_cnt + show_packet_count + 1) / 2), 2, indx - 1)
                #plt.xlabel("time")
            else:
                plt.subplot((int)((self.channel_cnt + show_packet_count + 1) / 2), 2, indx - 1, sharex = ax1)
            plt.ylabel(self.glp_header["ch_name"][indx - 2])
            plt.plot(self.data_rows[:, 0], self.data_rows[:, indx], colors[(indx - 2) % 7])
        if show_packet_count:
            plt.subplot((int)((self.channel_cnt + show_packet_count + 1) / 2), 2, indx, sharex = ax1)
            plt.ylabel("packetcount",)
            plt.plot(self.data_rows[:, 0], self.data_rows[:, 1])
        plt.xlabel("time (s)")
        plt.tight_layout()                              #reduce whitespace between panels to a minimum
        if show_plots:
            plt.show()
        plt.savefig(filename)
        plt.close(fig)

#################################### get size from file identifier ######################
    def size_from_type(self, filetype):
        if filetype == 'i' or filetype == 'I' or filetype == 'f' or filetype == 'l' or filetype == 'L':
            return 4
        elif filetype == 'b' or filetype == 'B' or filetype == 'c' or filetype == '?':
            return 1
        elif filetype == 'h' or filetype == 'H' or filetype == 'e':
            return 2
        elif filetype == 'q' or filetype == 'Q' or filetype == 'd':
            return 8
        else:
            return 8
        
#################################### get unit from unit identifier ######################        
    def unit_from_identifier(self, identifier):
        if identifier == 'l':
            return "lsb"
        elif identifier == 'g':
            return "g"
        elif identifier == 'd':
            return "dps"
        elif identifier == 'p':
            return "hPa"
        elif identifier == 'c':
            return "degC"
        elif identifier == 'h':
            return "%"
        elif identifier == 't':
            return "uT"
        elif identifier == 'm':
            return "mm"
        elif identifier == 'u':
            return "ms"
        
#### create config ###
if __name__=='__main__':
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('-f','--file-name',dest='filename',help='Specify BIN file name')
    arg_parser.add_argument('-p','--plot-data',dest='plot_choice',action='store_true',help='Plot data if specified')
    arg_parser.add_argument('-c', '--show-packet-count', dest='packet_cnt_choice', action='store_true', help='Show packet count if specified')
    arg_parser.add_argument('-s', '--save-plot', dest='save_plot_choice', action='store_true', help='Save plot to PNG file')
    arg_parser.add_argument('-o', '--output-file', dest='output_file',default=None,help='Save data to CSV/JSON file')
    args = arg_parser.parse_args()

    glpDecoder = GLP_Decoder()
    fileformat = os.path.splitext(args.output_file)[1][1:] if args.output_file is not None else ""
    plot_choice = args.plot_choice
    packet_cnt_choice = args.packet_cnt_choice
    save_plot_choice = args.save_plot_choice
    save_choice = True if args.output_file is not None else False
    altname = args.output_file
    filename = args.filename

    try:
        glpDecoder.reset()
        glpDecoder.read_bin(filename)
        if plot_choice is True:
            glpDecoder.plot_data(packet_cnt_choice)
        if save_plot_choice is True:
            glpDecoder.save_plot_data("exportfig",packet_cnt_choice)
        if save_choice is True:
            glpDecoder.make_file(filename,fileformat,altname)
    except Exception as e:
        print("problems reading data")
        print('Error on line {}'.format(sys.exc_info()[-1].tb_lineno), type(e).__name__, e)
        
