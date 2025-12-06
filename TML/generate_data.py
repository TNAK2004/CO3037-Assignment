import numpy as np
import pandas as pd


# Define ranges for temperature and humidity
temp_lower = 26
temp_upper = 30
humidity_lower = 40
humidity_upper = 60

# Function to generate a case for label 0 (normal)
def generate_normal_case():
    temperature = round(np.random.uniform(temp_lower, temp_upper), 5)
    humidity = round(np.random.uniform(humidity_lower, humidity_upper), 5)
    return temperature, humidity, 0

# Function to generate a case for label 1 (anomaly)
def generate_anomalous_case():
    # Randomly choose to perturb either temperature or humidity
    label = 1
    if np.random.rand() > 0.5:
        # Perturb temperature
        temperature = round(np.random.uniform(temp_lower - 5, temp_upper + 5), 5)
        humidity = round(np.random.uniform(humidity_lower, humidity_upper), 5)
    else:
        # Perturb humidity
        temperature = round(np.random.uniform(temp_lower, temp_upper), 5)
        humidity = round(np.random.uniform(humidity_lower - 10, humidity_upper + 10), 5)
    
    return temperature, humidity, label

# Number of cases for each label
num_cases = 2000  

normal_cases = [generate_normal_case() for _ in range(num_cases)]
anomalous_cases = [generate_anomalous_case() for _ in range(round(num_cases*0.5))]

# Combine both datasets
data = normal_cases + anomalous_cases

# Create a DataFrame
df = pd.DataFrame(data, columns=["temp", "humidity", "label"])

# Shuffle the dataset to mix the normal and anomalous cases
df = df.sample(frac=1, random_state=42).reset_index(drop=True)

# Save the DataFrame to a CSV file
df.to_csv("sensor_data.csv", index=False)

# Display the first few rows of the DataFrame to verify
print(df.head())

# Verify the balance of the labels
print("Label 0 count:", len(df[df['label'] == 0]))
print("Label 1 count:", len(df[df['label'] == 1]))