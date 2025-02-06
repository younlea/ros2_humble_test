To split the files in your `images` and `labels` folders into `train` and `val` subfolders with a 70:30 ratio while ensuring that files with the same base name are moved together, you can use a simple Bash script. Here's how you can achieve this:

### **Steps**

1. **Prepare the directory structure**:
   - Ensure you have the following directories:
     ```
     images/
     labels/
     ```
   - Create the output directories:
     ```
     mkdir -p images/train images/val labels/train labels/val
     ```

2. **Bash Script**:
   Use the following script to perform the split:

   ```bash
   #!/bin/bash

   # Paths to input and output directories
   IMAGES_DIR="images"
   LABELS_DIR="labels"
   TRAIN_IMAGES="images/train"
   VAL_IMAGES="images/val"
   TRAIN_LABELS="labels/train"
   VAL_LABELS="labels/val"

   # Create output directories if they don't exist
   mkdir -p "$TRAIN_IMAGES" "$VAL_IMAGES" "$TRAIN_LABELS" "$VAL_LABELS"

   # Get all file basenames (without extensions) from the images folder
   FILE_BASENAMES=$(find "$IMAGES_DIR" -type f -exec basename {} \; | sed 's/\.[^.]*$//' | sort | uniq)

   # Shuffle and split into 70% train and 30% val
   SHUFFLED_FILES=$(echo "$FILE_BASENAMES" | shuf)
   TOTAL_FILES=$(echo "$SHUFFLED_FILES" | wc -l)
   TRAIN_COUNT=$((TOTAL_FILES * 7 / 10))

   COUNTER=0
   for BASENAME in $SHUFFLED_FILES; do
       if [ $COUNTER -lt $TRAIN_COUNT ]; then
           mv "$IMAGES_DIR/$BASENAME".* "$TRAIN_IMAGES/" 2>/dev/null
           mv "$LABELS_DIR/$BASENAME".* "$TRAIN_LABELS/" 2>/dev/null
       else
           mv "$IMAGES_DIR/$BASENAME".* "$VAL_IMAGES/" 2>/dev/null
           mv "$LABELS_DIR/$BASENAME".* "$VAL_LABELS/" 2>/dev/null
       fi
       COUNTER=$((COUNTER + 1))
   done

   echo "Files successfully split into train and val sets."
   ```

3. **Explanation**:
   - The script identifies all unique file basenames in the `images` folder.
   - It shuffles the list of basenames to ensure randomness.
   - The total number of files is calculated, and 70% of them are assigned to the training set, while the remaining 30% go to the validation set.
   - Files are moved to their respective `train` or `val` subdirectories in both `images` and `labels`, ensuring that files with matching basenames are kept together.

4. **Run the script**:
   Save the script as `split_files.sh`, make it executable (`chmod +x split_files.sh`), and run it:
   ```bash
   ./split_files.sh
   ```

This method ensures that your dataset is split correctly while maintaining consistency between `images` and `labels`.

출처
[1] how to split into different percentages? - Unix & Linux Stack Exchange https://unix.stackexchange.com/questions/10219/split-how-to-split-into-different-percentages
[2] Splitting a file into multiple files based on 1st column value [duplicate] https://unix.stackexchange.com/questions/297683/splitting-a-file-into-multiple-files-based-on-1st-column-value
[3] Split folders with files (i.e. images) into training, validation and test ... https://github.com/jfilter/split-folders
[4] How to split the custom image dataset into training, testing, and ... https://python.plainenglish.io/how-to-split-the-custom-image-dataset-into-training-testing-and-validation-folders-for-machine-6b33c7b02c75?gi=ac6e0ff74470
[5] DL: Split Files into Training and Testing - GitHub Gist https://gist.github.com/stephen-hannam/0b58ba563a9e0966767c750bf13f0e81
[6] Solved: How do i split my dataset into 70% training , 30% testing https://communities.sas.com/t5/SAS-Data-Science/How-do-i-split-my-dataset-into-70-training-30-testing/td-p/131868
[7] How to split folder of images into test/training/validation sets with ... https://stackoverflow.com/questions/53074712/how-to-split-folder-of-images-into-test-training-validation-sets-with-stratified
[8] tools/bash/split-train-dev-test.sh at master - GitHub https://github.com/ye-kyaw-thu/tools/blob/master/bash/split-train-dev-test.sh
[9] Are there any tools in linux for splitting a file in place? - Super User https://superuser.com/questions/177823/are-there-any-tools-in-linux-for-splitting-a-file-in-place
[10] Split your directory automatically to train, validation & test folders https://www.kaggle.com/discussions/general/321868
[11] How can I split documents into training set and test set? https://stackoverflow.com/questions/42471570/how-can-i-split-documents-into-training-set-and-test-set
[12] Data preparation - Kaldi ASR https://kaldi-asr.org/doc/data_prep.html
[13] Train/Test/Validation Set Splitting in Sklearn https://datascience.stackexchange.com/questions/15135/train-test-validation-set-splitting-in-sklearn
[14] Train Test Split - How to split data into train and test for validating ... https://www.machinelearningplus.com/machine-learning/train-test-split/
[15] 221 - Easy way to split data on your disk into train, test, and validation? https://www.youtube.com/watch?v=C6wbr1jJvVs
[16] split a file, pass each piece as a param to a script, run each script in ... https://unix.stackexchange.com/questions/147111/split-a-file-pass-each-piece-as-a-param-to-a-script-run-each-script-in-paralle
[17] GAWK Manual - The GNU Awk User's Guide https://www.gnu.org/software/gawk/manual/gawk.html
[18] How can i find the train error with 70: 30 split on the data? https://stackoverflow.com/questions/65115609/how-can-i-find-the-train-error-with-70-30-split-on-the-data
[19] split training data and testing data - MATLAB Answers - MathWorks https://kr.mathworks.com/matlabcentral/answers/377839-split-training-data-and-testing-data
[20] Loading train/val/test datasets with images in separate folders using ... https://stackoverflow.com/questions/76082017/loading-train-val-test-datasets-with-images-in-separate-folders-using-pytorch/76082156
[21] Ultralytics YOLO11 - GitHub https://github.com/ultralytics/ultralytics/actions/runs/8029084347
[22] torchgeo.datasets - Read the Docs https://torchgeo.readthedocs.io/en/latest/api/datasets.html
[23] train test split 하는 커스텀 python 스크립트를 만들어봤습니다 https://walkaroundthedevelop.tistory.com/96
[24] [PDF] Rekognition - Custom Labels Guide - AWS Documentation https://docs.aws.amazon.com/pdfs/rekognition/latest/customlabels-dg/Rekognition%20Custom%20Labels.pdf
[25] How to split the custom image dataset into training, testing, and ... https://python.plainenglish.io/how-to-split-the-custom-image-dataset-into-training-testing-and-validation-folders-for-machine-6b33c7b02c75?gi=ac6e0ff74470
[26] How do I split correctly split my dataset into train, test and validation? https://datascience.stackexchange.com/questions/102463/how-do-i-split-correctly-split-my-dataset-into-train-test-and-validation
[27] how to split into different percentages? - Unix & Linux Stack Exchange https://unix.stackexchange.com/questions/10219/split-how-to-split-into-different-percentages
[28] Split folders with files (i.e. images) into training, validation and test ... https://github.com/jfilter/split-folders
[29] How to split data into train set and test set in R? - ProjectPro https://www.projectpro.io/recipes/split-data-into-train-set-and-test-set-r
[30] Solved: How do i split my dataset into 70% training , 30% testing https://communities.sas.com/t5/SAS-Data-Science/How-do-i-split-my-dataset-into-70-training-30-testing/td-p/131868
[31] How to split folder of images into test/training/validation sets with ... https://stackoverflow.com/questions/53074712/how-to-split-folder-of-images-into-test-training-validation-sets-with-stratified
[32] Split Your Dataset With scikit-learn's train_test_split() - Real Python https://realpython.com/train-test-split-python-data/
