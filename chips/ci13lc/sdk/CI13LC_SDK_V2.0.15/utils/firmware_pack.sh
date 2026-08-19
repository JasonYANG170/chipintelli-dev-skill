

case $CHIP_NAME in
    CI13080) ROM_SIZE=$((1*512*1024))
    ;;
    CI13081) ROM_SIZE=$((1*1024*1024))
    ;;
    CI13082) ROM_SIZE=$((2*1024*1024))
    ;;
    CI13160) ROM_SIZE=$((1*512*1024))
    ;;
    CI13161) ROM_SIZE=$((1*1024*1024))
    ;;
    CI13162) ROM_SIZE=$((2*1024*1024))
    ;;
    CI13240) ROM_SIZE=$((1*512*1024))
    ;;
    CI13241) ROM_SIZE=$((1*1024*1024))
    ;;
    CI13242) ROM_SIZE=$((2*1024*1024))
    ;;
    CI13320) ROM_SIZE=$((1*512*1024))
    ;;
    CI13321) ROM_SIZE=$((1*1024*1024))
    ;;
    CI13322) ROM_SIZE=$((2*1024*1024))
    ;;
    CI23161) ROM_SIZE=$((1*1024*1024))
    ;;
    CI23162) ROM_SIZE=$((2*1024*1024))
    ;;
    *) ROM_SIZE=$((2*1024*1024))
    ;;
esac

ASR_SIZE=`ls -l "../firmware/asr/asr.bin" | awk '{print $5}'`
CODE_SIZE=`ls -l "../firmware/user_code/user_code.bin" | awk '{print $5}'`
DNN_SIZE=`ls -l "../firmware/dnn/dnn.bin" | awk '{print $5}'`
VOICE_SIZE=`ls -l "../firmware/voice/voice.bin" | awk '{print $5}'`
USER_FILE_SIZE=`ls -l "../firmware/user_file/user_file.bin" | awk '{print $5}'`

ASR_SIZE=$((($ASR_SIZE+4095)/4096*4096))
CODE_SIZE=$((($CODE_SIZE+4095)/4096*4096))
DNN_SIZE=$((($DNN_SIZE+4095)/4096*4096))
VOICE_SIZE=$((($VOICE_SIZE+4095)/4096*4096))
USER_FILE_SIZE=$((($USER_FILE_SIZE+4095)/4096*4096))

echo ASR SIZE:$ASR_SIZE
echo CODE SIZE:$CODE_SIZE
echo NN SIZE:$DNN_SIZE
echo VOICE SIZE:$VOICE_SIZE
echo USER FILE SIZE:$USER_FILE_SIZE
../../../tools/ci-tool-kit mf -f v2\
    --user-code ../firmware/user_code/user_code.bin --user-code-size $CODE_SIZE\
    --asr-file ../firmware/asr/asr.bin --asr-size $ASR_SIZE\
    --nn-file ../firmware/dnn/dnn.bin --nn-size $DNN_SIZE\
    --voice-file ../firmware/voice/voice.bin --voice-size $VOICE_SIZE\
    --user-file ../firmware/user_file/user_file.bin --user-file-size $USER_FILE_SIZE\
    --rom-size $ROM_SIZE\
    --chip-name $CHIP_NAME\
    --output-path ../firmware

echo make firmware finished!
