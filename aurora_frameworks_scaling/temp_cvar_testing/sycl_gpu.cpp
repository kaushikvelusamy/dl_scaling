// module use /soft/modulefiles
// module load frameworks/2023.12.15.001 
// mpic++ -o sycl_gpu sycl_gpu.cpp -fsycl -lmpi -I/opt/aurora/23.275.2/oneapi/compiler/2024.0/include/sycl
// mpirun -np 2 -ppn 2 ./sycl_gpu
// unset ONEAPI_DEVICE_SELECTOR

#include <sycl/sycl.hpp> 
#include <mpi.h>
#include <cmath>

using namespace sycl;

int main(int argc, char** argv) 
{
    MPI_Init(&argc, &argv);
    int rank, nsize, res_len, provided;
    int cvar_index, cvar_nvals, value;
    char name[ MPI_MAX_PROCESSOR_NAME ];
    double t1, t2, t3, t4;

    MPI_T_cvar_handle cvar_handle;
    MPI_Comm_size( MPI_COMM_WORLD, &nsize );
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_T_init_thread( MPI_THREAD_FUNNELED, &provided );







    if ( rank == 0 )
    {   
        int num_cvar;     // Number of control variables
        char  varValue[512];

        MPI_Datatype datatype;
        MPI_T_cvar_get_num( &num_cvar);
        std::cout<<"Rank 0: number of control variables " << num_cvar << std::endl;

        int ival;
        unsigned uval;
        unsigned long ulval;
        unsigned long long ullval;

        for ( int i = 0; i < num_cvar; i++ ) 
        { 
            int namelen = 100, err, verbose, bind, scope; 
            err = MPI_T_cvar_get_info( i, name, &namelen, &verbose, &datatype, NULL, NULL, NULL, &bind, &scope );         
            MPI_T_cvar_get_index( name, &cvar_index );
            MPI_T_cvar_handle_alloc( cvar_index, NULL, &cvar_handle, &cvar_nvals );


            if (cvar_nvals == 1 || (datatype == MPI_CHAR && cvar_nvals < sizeof(varValue))) 
            {
                    if (MPI_INT == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, &ival);                        
                        std::cout << i << " " << name << " " << ival  << std::endl;
                    }  
                    else if (MPI_UNSIGNED == datatype)
                    {
                        MPI_T_cvar_read(cvar_handle, &uval);
                        std::cout << i << " " << name << " "  << uval  << std::endl;

                    } 
                    else if (MPI_UNSIGNED_LONG == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, &ulval);
                        std::cout << i << " " << name << " "  << ulval  << std::endl;

                    } 
                    else if (MPI_UNSIGNED_LONG_LONG == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, &ullval);
                        std::cout << i << " " << name << " "  << ullval  << std::endl;

                    } 
                    else if (MPI_CHAR == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, varValue);
                        std::cout << i << " " << name << " "  << varValue  << std::endl;
                    }
            }

        }
        
        MPI_T_cvar_handle_free( &cvar_handle );


    }









    sycl::queue Q(sycl::gpu_selector_v);
    
    // std::cout << "Running on " << Q.get_device().get_info<sycl::info::device::name>()  << std::endl;
    int  global_range;
    if (argc == 2)  
    {
       global_range = atoi(argv[1])/4;
    }
    else
    {
        global_range = 1048576;
    }
    auto *A  = sycl::malloc_device<float>(global_range,Q);
    auto *B  = sycl::malloc_device<float>(global_range,Q);

    Q.parallel_for(global_range, [=](sycl::item<1> id) { A[id] = 1; }).wait();
    Q.parallel_for(global_range, [=](sycl::item<1> id) { B[id] = 0; }).wait();    
    MPI_Barrier( MPI_COMM_WORLD ); 

    std::vector<double> elapsed(50);
    for (int i = 0; i < 5; i++)
    {
        t1 = MPI_Wtime();
        MPI_Allreduce(A, B, global_range, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Barrier( MPI_COMM_WORLD ); 
        t2 = MPI_Wtime();
        if ( rank == 0 )    elapsed[i]=( t2 - t1 ) * 1e6;
    }

 

    if ( rank == 0 )
    {   
        int num_cvar;     // Number of control variables
        char  varValue[512];

        MPI_Datatype datatype;
        MPI_T_cvar_get_num( &num_cvar);
        std::cout<<"Rank 0: number of control variables " << num_cvar << std::endl;

        int ival;
        unsigned uval;
        unsigned long ulval;
        unsigned long long ullval;

        for ( int i = 0; i < num_cvar; i++ ) 
        { 
            int namelen = 100, err, verbose, bind, scope; 
            err = MPI_T_cvar_get_info( i, name, &namelen, &verbose, &datatype, NULL, NULL, NULL, &bind, &scope );         
            MPI_T_cvar_get_index( name, &cvar_index );
            MPI_T_cvar_handle_alloc( cvar_index, NULL, &cvar_handle, &cvar_nvals );


            if (cvar_nvals == 1 || (datatype == MPI_CHAR && cvar_nvals < sizeof(varValue))) 
            {
                    if (MPI_INT == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, &ival);                        
                        std::cout << i << " " << name << " " << ival  << std::endl;
                    }  
                    else if (MPI_UNSIGNED == datatype)
                    {
                        MPI_T_cvar_read(cvar_handle, &uval);
                        std::cout << i << " " << name << " "  << uval  << std::endl;

                    } 
                    else if (MPI_UNSIGNED_LONG == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, &ulval);
                        std::cout << i << " " << name << " "  << ulval  << std::endl;

                    } 
                    else if (MPI_UNSIGNED_LONG_LONG == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, &ullval);
                        std::cout << i << " " << name << " "  << ullval  << std::endl;

                    } 
                    else if (MPI_CHAR == datatype) 
                    {
                        MPI_T_cvar_read(cvar_handle, varValue);
                        std::cout << i << " " << name << " "  << varValue  << std::endl;
                    }
            }
        }
        MPI_T_cvar_handle_free( &cvar_handle );
    }


    MPI_Finalize();
    return 0;
}
