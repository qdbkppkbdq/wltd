set -x

# GPUS=${GPUS:-4}
BATCH_SIZE=${BATCH_SIZE:-32}
WEILAI_NNODES=${WEILAI_NNODES:-1}
WEILAI_NODE_RANK=${WEILAI_NODE_RANK:-0}
WEILAI_MASTER_ADDR=${WEILAI_MASTER_ADDR:-127.0.0.1}
WEILAI_MASTER_PORT=${WEILAI_MASTER_PORT:-34229}
WEILAI_GPUS=${WEILAI_GPUS:-8}
echo "WEILAI_NNODES: ${WEILAI_NNODES}"
echo "WEILAI_NODE_RANK: ${WEILAI_NODE_RANK}"
echo "WEILAI_MASTER_ADDR: ${WEILAI_MASTER_ADDR}"
echo "WEILAI_MASTER_PORT: ${WEILAI_MASTER_PORT}"
echo "WEILAI_GPUS: ${WEILAI_GPUS}"
export PYTHONPATH="${PYTHONPATH}:$(pwd)"
export TF_CPP_MIN_LOG_LEVEL=3
export LAUNCHER=pytorch

OUTPUT_DIR='work_dirs/internvl_stage2_finetune_flickr_364_bs1024_ep10_lora_4gpu'

if [ ! -d "$OUTPUT_DIR" ]; then
  mkdir -p "$OUTPUT_DIR"
fi
#flickr30k_en_train
# number of gpus: 32
# batch size per gpu: 32
# gradient accumulation steps: 1
# total batch size: 1024
# epoch: 10
torchrun \
  --nnodes=${WEILAI_NNODES} \
  --node_rank=${WEILAI_NODE_RANK} \
  --master_addr=${WEILAI_MASTER_ADDR} \
  --nproc_per_node=${WEILAI_GPUS} \
  --master_port=${WEILAI_MASTER_PORT} \
  internvl/train/internvl_stage2_finetune.py \
  --dataset_name ${DATASET_NAME} \
  --model_name_or_path "./pretrained/InternVL-14B-224px" \
  --output_dir ${OUTPUT_DIR} \
  --overwrite_output_dir True \
  --freeze_model \
  --freeze_vision_model \
  --freeze_qllama \
  --unfreeze_qllama_head \
  --use_backbone_lora 16 \
  --use_qllama_lora 16 \
  --force_image_size 224 \
  --drop_path_rate 0.0 \
  --dataloader_num_workers 2 \
  --pad_to_max_length True \
  --bf16 True \
  --num_train_epochs 10 \
  --per_device_train_batch_size ${BATCH_SIZE} \
  --gradient_accumulation_steps 1 \
  --evaluation_strategy "no" \
  --save_strategy "steps" \
  --save_steps 100 \
  --save_total_limit 5 \
  --learning_rate 1e-6 \
  --weight_decay 0.05 \
  --warmup_steps 100 \
  --lr_scheduler_type "cosine" \
  --logging_steps 1 \
  --max_seq_length 80 \
  --do_train True \
  --optim adamw_torch \
  --deepspeed "zero_stage3_config.json" \
  --report_to "tensorboard"