import subprocess
import argparse
import os
import sys
import math
import time
import re
import copy
import json

def write_batch_script(file_name, ntasks, cpus, executable, dataset_path, cities):
    f = open(file_name + ".sh", "w+")
    f.write("#!/bin/bash\n")
    f.write("#SBATCH -p compute # partition (queue)\n")
    f.write("#SBATCH --job-name=" + file_name + "\n")
    f.write("#SBATCH --ntasks=" + str(ntasks) + "\n")
    f.write("#SBATCH --cpus-per-task=" + str(cpus) + "\n")
    f.write("#SBATCH --nodes=" + str(ntasks) + "\n")
    if (cities == 26):
        f.write("#SBATCH --time=2-00:00" + "# time (D-HH:MM) \n")
    elif (cities > 15):
        f.write("#SBATCH --time=0-10:00" + "# time (D-HH:MM) \n")
    else:
        f.write("#SBATCH --time=0-00:30" + "# time (D-HH:MM) \n")
    #f.write("#SBATCH --output=mpi_test_%j.log")
    f.write("#SBATCH --output=" + file_name + "_%j.out" + "\n")
    f.write("#SBATCH -e %j.err" + "\n")
    f.write("\n")
    f.write("export OMP_NUM_THREADS=" + str(cpus) + "\n")
    f.write("export MV2_ENABLE_AFFINITY=0" + "\n")

    command = "mpirun ./" + executable + " --dataset=" + dataset_path + " --cities=" + str(cities) + \
                " --outfile=" + file_name + "_" + str(slurm_job_id)
    #print(command)
    f.write(command + "\n")
    f.close()

def clear_files(run_dict):
    print("Cleaning up files - launch_scripts, output file for each node except 0")
    for jobid in run_dict['jobids']:
        if (os.path.exists(run_dict[jobid]['launch_script'])):
            os.remove(run_dict[jobid]['launch_script'])

        for file in run_dict[jobid]['node_output_files']:
            #dont delete the master node file
            if "_0.txt" not in file:
                if (os.path.exists(file)):
                    os.remove(file)

        if (os.path.exists(cancel_script_name)):
            os.remove(cancel_script_name)


def display_results(run_dict, serial_time):
    print("\n\nCONFIGURATION\t\tRUNTIME(sec)\t\tSPEEDUP")
    print("========================================================================")

    for jobid in run_dict['jobids']:
        print(run_dict[jobid]['name'] + "\t\t" + str(run_dict[jobid]['time']) + "\t\t" + str(serial_time/run_dict[jobid]['time']))
        print("========================================================================")
    clear_files(run_dict)


def check_and_wait(run_dict, serial_time):
    run_dict['running'] = copy.deepcopy(run_dict['jobids'])
    count_not_done = len(run_dict['running'])

    while count_not_done > 0:
        response = subprocess.run(["squeue", "-u", "r4gupta"], check=True, stdout=subprocess.PIPE,
                                universal_newlines=True)
        output = response.stdout

        for jobid in run_dict['running']:
            if (str(jobid) not in output):
                run_dict['running'].remove(jobid)
                count_not_done -= 1
                #check the output
                if (os.path.exists(run_dict[jobid]['outfile'])):
                    job_response = subprocess.run(["tail", "-10", run_dict[jobid]['outfile']], check=True, stdout=subprocess.PIPE,
                                universal_newlines=True)
                    job_output = job_response.stdout
                    #print(job_output)
                    pattern = re.compile("Wallclock time = \d+.\d+")
                    result = pattern.search(job_output)
                    run_dict[jobid]['time'] = float(result.group(0).split(' ')[-1])

        if (count_not_done != 0):
            temp_str = ' '.join(str(e) for e in run_dict['running'])
            print("Sleeping for 30 seconds as " +  str(count_not_done) + " jobs are left:" + temp_str)
            time.sleep(30)
        else:
            break
    display_results(run_dict, serial_time)
    #print(run_dict)


def launch_commands(executable, iterations, dataset_path, cities, serial_time):
    run_dict = {}
    run_dict['jobids'] = []
    for nt in range (1, cities):
        cpus = int(math.ceil((cities-1) * 1.0/nt))

        for i in range(1, iterations+1):
            file_name = executable + "_n" + str(nt) + "_c" + str(cpus) + "_i" + str(i)
            write_batch_script(file_name, nt, cpus, executable, dataset_path, cities)
            try:
                response = subprocess.run(["sbatch", file_name + ".sh"], check=True, stdout=subprocess.PIPE, universal_newlines=True)
            except subprocess.CalledProcessError:
                sys.exit("error while creating " + file_name)
            output = response.stdout
            #output = 'Submitted batch job 23766845'
            #print(output)
            jobid = int(output.split(' ')[-1])

            #write to cancel script
            cancel_fd.write("scancel " + str(jobid) + "\n")

            run_dict[jobid] = {}
            run_dict[jobid]['name'] = "CORES:" + str(nt) + ",CPUS:" + str(cpus)
            run_dict[jobid]['outfile'] = file_name + "_" + str(jobid) + ".out"
            run_dict[jobid]['launch_script'] = file_name + ".sh"
            run_dict[jobid]['node_output_files'] = []
            for j in range(nt):
                run_dict[jobid]['node_output_files'].append(file_name + "_" + str(slurm_job_id) + "_" + str(j) + ".txt")
            run_dict['jobids'].append(jobid)
            time.sleep(1)

            #response = subprocess.run(["scancel", str(jobid)])
    #print(run_dict)
    cancel_fd.close()
    check_and_wait(run_dict, serial_time)



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Optional app description')
    parser.add_argument('--executable', type=str, nargs='+', required=True,
                        help='Executable to use')
    parser.add_argument('--cities', type=int,
                        help='Dataset to use:5')
    parser.add_argument('--iteration', type=int, nargs='?',const=5,
                        help='Iterations per configuration, default:5')



    args = parser.parse_args()
    if args.cities == None:
        args.cities = 5
    if args.cities not in [5, 11, 15, 17, 26]:
        sys.exit("Error! We only have datasets with cities - 5, 11, 15, 17, 26")
    if args.iteration == None:
        args.iteration = 1

    print("args.cities:", args.cities)
    print("args.iteration:", args.iteration)
    print("args.executable:", args.executable[0])
    print("--iteration argument is not supported yet. It defaults to 1 nevertheless")

    cities = args.cities
    iteration = args.iteration
    executable = args.executable[0]

    cwd = os.getcwd()
    print("currently in " + cwd)

    if ('SLURM_JOB_ID' not in os.environ.keys()):
        sys.exit("Please run it using sbatch on a single core!")

    if os.path.exists(executable) == False:
        sys.exit("Executable does not exist")

    slurm_job_id = os.environ['SLURM_JOB_ID']

    cancel_script_name = "cancel_" + str(slurm_job_id) + ".sh"
    print("\nRun the following command to cancel all the launched jobs - ./" + cancel_script_name + "\n")
    cancel_fd = open(cancel_script_name, "w+")
    #make it executable
    subprocess.run(["chmod", "+x", cancel_script_name], check=True)
    cancel_fd.write("#!/bin/bash\n")

    datasets = {}
    datasets[5] = {}
    datasets[5]['path'] = "./datasets/five_d.txt"
    datasets[5]['serial_time'] = 0.000059

    datasets[11] = {}
    datasets[11]['path'] = "./datasets/custom_11_d.txt"
    datasets[11]['serial_time'] = 2.365838

    datasets[15] = {}
    datasets[15]['path'] = "./datasets/lau15_dist_15c_d.txt"
    datasets[15]['serial_time'] = 145.732252

    datasets[17] = {}
    datasets[17]['path'] = "./datasets/gr17_d.txt"
    #TODO this is not the serial time. Estimate it
    datasets[17]['serial_time'] = 7116.46887

    datasets[26] = {}
    datasets[26]['path'] = "./datasets/gr17_d.txt"
    #TODO this is not the serial time. Estimate it
    datasets[26]['serial_time'] = 158400

    dataset_path = datasets[cities]['path']
    serial_time = datasets[cities]['serial_time']
    if os.path.exists(dataset_path) == False:
        sys.exit("dataset path " + dataset_path + " does not exist")

    launch_commands(executable, 1, dataset_path, cities, serial_time)


