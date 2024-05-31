#!/bin/bash -x
# qsub -l nodes=100 -q workq  -l walltime=02:00:00 -l filesystems=gila -A  Aurora_deployment 
#PBS -A Aurora_deployment
#PBS -k doe

module use /soft/modulefiles
module load frameworks/2023.12.15.001 
module list

cd $PBS_O_WORKDIR

echo Jobid: $PBS_JOBID
echo Running on nodes `cat $PBS_NODEFILE`
# NNODES=`wc -l < $PBS_NODEFILE`
cp $PBS_NODEFILE ~/
chmod 755 ~/$PBS_JOBID

# sed -i '/x1922c6s2b0n0/d' ~/$PBS_JOBID
# sed -i '/x1921c0s7b0n0/d' ~/$PBS_JOBID
# sed -i '/x1922c5s2b0n0/d' ~/$PBS_JOBID
# sed -i '/x1922c6s1b0n0/d' ~/$PBS_JOBID
# sed -i '/x1922c1s2b0n0/d' ~/$PBS_JOBID
# sed -i '/x1921c0s0b0n0/d' ~/$PBS_JOBID
# sed -i '/x1921c6s7b0n0/d' ~/$PBS_JOBID

wc -l ~/$PBS_JOBID

NNODES=8
RANKS_PER_NODE=12          # Number of MPI ranks per node
NRANKS=$(( NNODES * RANKS_PER_NODE ))
echo "NUM_OF_NODES=${NNODES}  TOTAL_NUM_RANKS=${NRANKS}  RANKS_PER_NODE=${RANKS_PER_NODE}"
# CPU_BINDING1=list:3:4:11:12:19:20:27:28:35:36:43:44:55:56:63:64:71:72:79:80:87:88:95:96 
CPU_BINDING1=list:4:9:14:19:20:25:56:61:66:71:74:79
EXT_ENV="--env FI_CXI_DEFAULT_CQ_SIZE=1048576"
APP1=/lus/gila/projects/CSC250STDM10_CNDA/kaushik/oneCCL/build/_install/examples/benchmark/benchmark

which python
mkdir -p ./out_${PBS_JOBID}/c_mpi

unset ONEAPI_DEVICE_SELECTOR

RANKS_PER_NODE=12          # Number of MPI ranks per node
NRANKS=$(( NNODES * RANKS_PER_NODE ))
    # for BUF_SIZE in 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216  33554432 67108864 134217728

    for BUF_SIZE in 512 1024 2048 4096 262144 524288 1048576 2097152 4194304 
    do
        date
            echo ${BUF_SIZE}
            mpiexec ${EXT_ENV} --np ${NRANKS} -ppn ${RANKS_PER_NODE}  --cpu-bind  $CPU_BINDING1  ./sycl_gpu ${BUF_SIZE} > ./out_${PBS_JOBID}/c_mpi/${PBS_JOBID}_${NNODES}_${NRANKS}_${RANKS_PER_NODE}_${BUF_SIZE}_c_mpi.txt
            # mpiexec --hostfile ~/$PBS_JOBID  ${EXT_ENV} --np ${NRANKS} -ppn ${RANKS_PER_NODE}  --cpu-bind  $CPU_BINDING1  ./sycl_gpu ${BUF_SIZE} > ./out_${PBS_JOBID}/c_mpi/${PBS_JOBID}_${NNODES}_${NRANKS}_${RANKS_PER_NODE}_${BUF_SIZE}_c_mpi.txt
        date
    echo ${BUF_SIZE}
 
    done

