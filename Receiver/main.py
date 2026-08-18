"""
import serial 
import struct
ser = serial.Serial('COM13', 9600)  #Open communication
FORMAT_STRUCT = "<fffffffQfffIhhh" #what are the types of data spected inside the struct
SIZE_STRUCT = 58 #Number of bits in the message 
while ser.is_open: #only works when the serial is open
    if ser.read(1) == b'#': #checks for the first # in the header 

        if ser.read(1) == b'#':# checks for the second # in the header

            raw_data = ser.read(SIZE_STRUCT) #takes the raw data from the message
            
            if len(raw_data) == SIZE_STRUCT: #makes sure the sizes match
                lidar = struct.unpack(FORMAT_STRUCT, raw_data) #converts them into USEFUL DATA 
                print(lidar[2])
"""
import os
os.environ['PGCLIENTENCODING'] = 'WIN1252'  # Trata mensagens nativas do Windows
import psycopg2
from Receiver import *

DB_info={##Information needed to connect to the database
    "dbname":"Dynamics",
    "user":"postgres",
    "password":"123",
    "host": "localhost",
    "port": "5432"
}

##Command to insert the data into the database
INSERT_QUERY = """ 
INSERT INTO sensor_telemetry (
    ax, ay, az, gx, gy, gz, temp_i, time_raw,
    temp_o, humidity, pressure, gas, mx, my, mz
) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
"""
x=Receiver('COM13', 9600,"<fffffffQfffIhhh",58)#Creates the receiver object with the needed information
c=0
try:
    # Abre a conexão com o banco de dados
    conn = psycopg2.connect(**DB_info)
    cursor = conn.cursor()
    print("Conectado ao PostgreSQL com sucesso!")
    while x.ser.is_open:
        d = x.store()
        if d is not None:
            c=c+1
            print(c)
            cursor.execute(INSERT_QUERY, d) # Envia a tupla d diretamente para os %s
            conn.commit()                   # GRAVAÇÃO DINÂMICA: Salva no HD a cada leitura!
            '''if(c>=1000):
                x.ser.close()
                break   '''
except Exception as e:
    print(f"Erro na execução: {e}")              
finally:
    if 'cursor' in locals():
        cursor.close()
    if 'conn' in locals():
        conn.close()
    if x.ser.is_open:
        x.ser.close()
    print("Conexões encerradas com segurança.")
