  #Electrónica Digital 2
  #Proyecto 2: Videojuego
  #Autores: Carlos Valdés
           #Estuardo Castillo
  #Nombre del programa: serial_threading.py
  #descripción: Programa para crear canales de comunicacion entre controles y TIVAc 
  #Se utiliza threading para que los procesos puedan ser ejecutados en paralelo, un control independiente al otro, con el mismo target o destino. 
  #Hardware: COM fisico

import serial
import threading

def forward_data(source_port, destination_port):
    while True:
        data = source_port.read(1)
        if data:
            first_character = data.decode('utf-8', errors='ignore')[0]
            destination_port.write(data)
            print(f"Transmitted: {first_character}")
def main():
    # Intentar abrir coms
    try:
        com11 = serial.Serial('COM15', 115200)
        com12 = serial.Serial('COM12', 115200)
        com5 = serial.Serial('COM5', 115200)
        
        print("All COM ports connected successfully.")

        # Creación de hilos para mandar datos
        thread1 = threading.Thread(target=forward_data, args=(com11, com5))
        thread2 = threading.Thread(target=forward_data, args=(com12, com5))

        # Iniciar hilos 
        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        # cerrar al terminar
        com11.close()
        com12.close()
        com5.close()
    #manejar excepción si un puerto no responde 
    except serial.SerialException as e:
        print(f"Failed to connect to COM ports: {e}")

if __name__ == "__main__":
    main()
