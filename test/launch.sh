#!/bin/bash
HOSTS_FILE="hosts.txt"             # 存放机器 IP 的文件
DOCKER_IMAGE="internvl:v1"   # Docker 镜像名称
SCRIPT_IN_DOCKER="shell/lora_finetune/internvl_stage2_finetune_flickrcn_224_bs1024_ep10_lora16_4gpu.sh"  # 容器中的训练脚本路径
MASTER_ADDR="10.0.200.182"       # 主节点地址 (第一台机器 IP)
MASTER_PORT="34229"               # 通信的端口号
GPUS_PER_NODE=8                   # 每台机器的 GPU 数量
NNODES=$(wc -l < $HOSTS_FILE)     # 总节点数 (行数)
echo "检测到的节点数量: $NNODES"
echo "hosts.txt 文件内容:"
cat $HOSTS_FILE
LOG_DIR="./logs"                  # 日志存放目录
LOG_PREFIX="training_log"         # 日志文件前缀
DATASET_NAME="flickr30k_en_train"
OUTPUT_DIR="./work_dirs/internvl_stage2_finetune_flickrcn_364_bs1024_ep10_lora_4gpu"
BATCH_SIZE=1024
# 在脚本开始处添加
if [ ! -f "$HOSTS_FILE" ]; then
    echo "错误：hosts 文件 ($HOSTS_FILE) 不存在"
    exit 1
fi

if [ $NNODES -eq 0 ]; then
    echo "错误：hosts 文件为空"
    exit 1
fi

# 配置参数


# 创建日志目录
mkdir -p $LOG_DIR

# 初始化节点 RANK
NODE_RANK=0

# 使用数组方式读取hosts文件
mapfile -t hosts < "$HOSTS_FILE"
for HOST in "${hosts[@]}"; do
    echo "正在处理主机: $HOST"
    echo "Starting training script on $HOST (NODE_RANK=$NODE_RANK)..."

    # 为当前节点生成日志文件
    LOG_FILE="${LOG_DIR}/${LOG_PREFIX}_node${NODE_RANK}.log"

    # SSH 到目标机器，清理已存在的容器
    ssh $HOST "docker rm -f weilai_internvl 2>/dev/null || true"

    # SSH 到目标机器，启动 Docker 容器并将日志输出到本机
    cmd="docker run -d --rm --gpus all \
        -v /DATA/disk0/InternVL:/root/InternVL \
        -e WEILAI_NNODES=$NNODES \
        -e WEILAI_NODE_RANK=$NODE_RANK \
        -e WEILAI_MASTER_ADDR=$MASTER_ADDR \
        -e WEILAI_MASTER_PORT=$MASTER_PORT \
        -e WEILAI_GPUS=$GPUS_PER_NODE \
        -e WEILAI_DATASET_NAME=$DATASET_NAME \
        -e WEILAI_OUTPUT_DIR=$OUTPUT_DIR \
        -e WEILAI_BATCH_SIZE=$BATCH_SIZE \
        -e NCCL_DEBUG=INFO \
        -e NCCL_DEBUG_SUBSYS=ALL \
        -e NCCL_DEBUG_FILE=/root/InternVL/nccl.log.%h.%p \
        -e NCCL_IB_GID_INDEX=3 \
        --name weilai_internvl \
        $DOCKER_IMAGE \
        bash -c \"cd /root/InternVL/internvl_g && sh $SCRIPT_IN_DOCKER\" "

    echo "Executing command on $HOST:" >> $LOG_FILE
    echo "$cmd" >> $LOG_FILE
    ssh $HOST "$cmd" >> $LOG_FILE 2>&1 &
    # 打印日志文件路径
    echo "Log for NODE_RANK=$NODE_RANK written to $LOG_FILE"

    # 增加节点的 RANK
    NODE_RANK=$((NODE_RANK+1))
done

# 提示任务部署完成
echo "All nodes have been initialized. Logs are stored in $LOG_DIR."