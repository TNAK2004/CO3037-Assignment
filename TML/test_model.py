import pandas as pd
import tensorflow as tf

data = pd.read_csv("test.csv", names=["temp", "humidity", "label"])
X_test = data[["temp", "humidity"]].values
y_test = data["label"].values

model = tf.keras.models.load_model("TinyML.h5")

print(f"{model.predict(X_test)}")