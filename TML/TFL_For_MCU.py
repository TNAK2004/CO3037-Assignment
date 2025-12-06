import pandas as pd
from sklearn.model_selection import train_test_split
import tensorflow as tf

PREFIX = "TinyML"

data = pd.read_csv("sensor_data.csv", names=["temp", "humidity", "label"])
X = data[["temp", "humidity"]].values
y = data["label"].values

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.1, random_state=42)
X_train, X_val, y_train, y_val = train_test_split(X_train, y_train, test_size=0.1, random_state=42)

# Simple classifier model
model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(2,)),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dropout(0.1),
    tf.keras.layers.Dense(16, activation='relu'),
    tf.keras.layers.Dense(8, activation='relu'),
    tf.keras.layers.Dense(1, activation='sigmoid')
])

model.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])
model.fit(X_train, y_train, epochs=100, batch_size=128, shuffle=True, validation_data=(X_val, y_val))
model.save(PREFIX + '.h5')

print(model.evaluate(X_test, y_test, batch_size=64))

# Convert to TFLite
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
tflite_model = converter.convert()

with open(PREFIX + ".tflite", "wb") as f:
    f.write(tflite_model)

tflite_path = PREFIX + '.tflite'
output_header_path = 'dht_anomaly_model.h'

with open(tflite_path, 'rb') as tflite_file:
    tflite_content = tflite_file.read()

hex_lines = [', '.join([f'0x{byte:02x}' for byte in tflite_content[i:i + 12]]) for i in
         range(0, len(tflite_content), 12)]

hex_array = ',\n  '.join(hex_lines)

with open(output_header_path, 'w') as header_file:
    header_file.write('const unsigned char dht_anomaly_model_tflite[] = {\n  ')
    header_file.write(f'{hex_array}\n')
    header_file.write('};\n\n')