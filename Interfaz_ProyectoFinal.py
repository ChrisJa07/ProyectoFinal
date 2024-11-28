import tkinter as tk
from tkinter import ttk
from paho.mqtt.client import Client

# Configuración MQTT
BROKER = "broker.emqx.io"
PORT = 1883
TOPIC_PWM = "esp32/pwm"
TOPIC_LED1 = "esp32/led1"
TOPIC_LED2 = "esp32/led2"
TOPIC_ADC = "esp32/adc"

# Cliente MQTT
client = Client()

# Funciones MQTT
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        log_text.insert(tk.END, "Conectado al broker MQTT\n")
        client.subscribe(TOPIC_ADC)
    else:
        log_text.insert(tk.END, f"Error al conectar, código: {rc}\n")

def on_message(client, userdata, msg):
    if msg.topic == TOPIC_ADC:
        adc_value.set(f"ADC: {msg.payload.decode()}")
        log_text.insert(tk.END, f"Valor ADC recibido: {msg.payload.decode()}\n")

def connect_broker():
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT, 60)
    client.loop_start()
    log_text.insert(tk.END, "Intentando conectar al broker...\n")

# Funciones de control
def enviar_pwm():
    valor = pwm_slider.get()
    client.publish(TOPIC_PWM, valor)
    log_text.insert(tk.END, f"PWM enviado: {valor}\n")

def enviar_led1(estado):
    client.publish(TOPIC_LED1, estado)
    log_text.insert(tk.END, f"LED1 {estado} enviado\n")

def enviar_led2(estado):
    client.publish(TOPIC_LED2, estado)
    log_text.insert(tk.END, f"LED2 {estado} enviado\n")

# Interfaz gráfica
root = tk.Tk()
root.title("Cliente MQTT - ESP32")

# Conexión al broker
frame_broker = ttk.LabelFrame(root, text="Broker MQTT")
frame_broker.pack(padx=10, pady=5, fill="x")
ttk.Label(frame_broker, text="Broker:").pack(side="left", padx=5)
ttk.Label(frame_broker, text=BROKER).pack(side="left", padx=5)
ttk.Button(frame_broker, text="Conectar", command=connect_broker).pack(side="right", padx=5)

# Control de LEDs
frame_leds = ttk.LabelFrame(root, text="Control de LEDs")
frame_leds.pack(padx=10, pady=5, fill="x")
ttk.Label(frame_leds, text="LED1:").pack(side="left", padx=5)
ttk.Button(frame_leds, text="ON", command=lambda: enviar_led1("ON")).pack(side="left", padx=5)
ttk.Button(frame_leds, text="OFF", command=lambda: enviar_led1("OFF")).pack(side="left", padx=5)
ttk.Label(frame_leds, text="LED2:").pack(side="left", padx=5)
ttk.Button(frame_leds, text="ON", command=lambda: enviar_led2("ON")).pack(side="left", padx=5)
ttk.Button(frame_leds, text="OFF", command=lambda: enviar_led2("OFF")).pack(side="left", padx=5)

# Control del PWM
frame_pwm = ttk.LabelFrame(root, text="Control PWM")
frame_pwm.pack(padx=10, pady=5, fill="x")
pwm_slider = ttk.Scale(frame_pwm, from_=0, to=255, orient="horizontal")
pwm_slider.pack(side="left", padx=5, fill="x", expand=True)
ttk.Button(frame_pwm, text="Enviar PWM", command=enviar_pwm).pack(side="right", padx=5)

# Lectura del ADC
frame_adc = ttk.LabelFrame(root, text="Lectura del ADC")
frame_adc.pack(padx=10, pady=5, fill="x")
adc_value = tk.StringVar(value="ADC: ---")
ttk.Label(frame_adc, textvariable=adc_value).pack(side="left", padx=5)

# Log de mensajes
frame_log = ttk.LabelFrame(root, text="Log de Mensajes")
frame_log.pack(padx=10, pady=5, fill="both", expand=True)
log_text = tk.Text(frame_log, height=10)
log_text.pack(fill="both", expand=True)

# Iniciar la interfaz
root.mainloop()
