# Robustness_for_SVD
Robustness for Software Vulnerability Detection
# Robustness_for_SVD
Robustness for Software Vulnerability Detection
# A novel robustness verification method for source code processing based artificial intelligence systems

## Main research content

To solve the problem of instability of AI systems based on source code processing in complex code scenarios, this paper proposes a Robustness Verification method (RVMHM, Robustness Verification by Metropolis-Hastings attack).  Firstly, the code preprocessing tool based on the program’s abstract syntax trees is used to extract the variable pool of the model, and then the MHM source code attack algorithm is used to replace the variable perturbation model prediction effect.  The robustness of the AI system is measured by observing the change in the robustness verification index before and after the attack by interfering with the data and model interaction process.

```
The address of the research is ./RobTest

```

## The Project Structure
```
Robustness for Software Vulnerability Detection
|--data # the dataset file
    |--FFmpeg # example dataset which is already preprocessed
|--adv_model # the adversarial model file
    |--config # the setting file
    # You can modify the hyperparameters of the corresponding algorithm (maximum iteration, runtime, attack range) here.
    # At the same time, you need to register the information of your model in the setting file to ensure that the algorithm can use your model. 
    # At present, this project only supports the vulnerability detection model trained by TensorFlow framework. There are Models examples in "./victim_models".
    |--embedding # feature embedding extraction encoding mapped data files
    |--tools # utility functions used by some projects
    |--victim_models # The model to be validated
    |--log & mid_data & preprocessdata #The verbose data
|--dataset.py
|--generate_datastruct.py #the file that generates data structures
|--Robust.py Attack algorithm template
|--mhm.py MHM main file
```

## How to Conduct the Project

You can simply run the project by this code

```python
python RVmhm.py
```
The code can show the running process and performance on the open source dataset FFmpeg

If you want to run other data sets, you need to reset your Embbeding algorithm save file, your model save file, and your preprocessed data set by modifying config/config.yaml.

The preprocessed datasets are available through the open source tool Codesensor, which is linked to the Codesensor project below.
https://github.com/fabsx00/codesensor

After finishing setting the above data  and model files are set up, you can conduct the main program RVmhm.

```python
python RVmhm.py
```
