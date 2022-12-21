#!/bin/bash

# Bash Menu Script Example
PS3="Select your option by entering in a number: "
options_array=(Problem-1a Problem-1b Problem-1c Problem-2a Problem-2b Problem-3 Quit)
problem1c_array=(Serial Parallel Quit)
problem2a_array=(Blocking Immediate Quit)
proc=$(nproc --all)

select option in "${options_array[@]}";
do
    case $option in
        "Problem-1a")
            echo "Selected item $option $REPLY"
            echo "Number of Cores on this machine: ${proc}"
            echo "Enter in the number of processors n:"
            read n
            echo "Selected number of processors n:${n}"
            mpirun -n $n ./p1a.out;;
        "Problem-1b")
           echo "Selected item $option $REPLY"
           echo "Number of Cores on this machine: ${proc}"
           echo "Enter in the number of processors n:"
           read n
           echo "Selected number of processors n:${n}"
           mpirun -n $n ./p1b.out;;
        "Problem-1c")
            echo "Selected item $option $REPLY"
            echo "Number of Cores on this machine: ${proc}"
            echo "Select your option by entering in a number:"
            select option in "${problem1c_array[@]}";
               do
               case $option in
               "Serial")
               echo "Enter in the number of processors n:"
               read n
               echo "Selected number of processors n:${n}"
               mpirun -n $n ./serial_p1c.out;;
               "Parallel")
               echo "Enter in the number of processors n:"
               read n
               echo "Selected number of processors n:${n}"
               mpirun -n $n ./parallel_p1c.out;;
               "Quit")
               echo "We're done"
               break;;
               *)
               echo "invalid option $REPLY";
               esac
               done
            ;;
        "Problem-2a")
            echo "Selected item $option $REPLY"
            echo "Number of Cores on this machine: ${proc}"
            echo "Select your option by entering in a number:"
            select option in "${problem2a_array[@]}";
               do
               case $option in
               "Blocking")
               echo "Enter in the number of processors n:"
               read n
               echo "Selected number of processors n:${n}"
               mpirun -n $n ./p2a_blocking.out;;
               "Immediate")
                  echo "Enter in the number of processors n:"
                  read n
                  echo "Selected number of processors n:${n}"
                  mpirun -n $n ./p2a_immediate.out;;
               "Quit")
               echo "We're done"
               break;;
               *)
               echo "invalid option $REPLY";
               esac
               done
               ;;
        "Problem-2b")
         echo "Selected item $option $REPLY"
           echo "Number of Cores on this machine: ${proc}"
           echo "Enter in the number of processors n:"
           read n
           echo "Selected number of processors n:${n}"
           mpirun -n $n ./p2b.out;;
        "Problem-3")
           echo "Selected item $option $REPLY";;
        "Quit")
           echo "We're done"
           break;;
        *)
           echo "invalid option $REPLY";
    esac
done

#Sources: https://askubuntu.com/questions/1705/how-can-i-create-a-select-menu-in-a-shell-script
#Sources: https://www.baeldung.com/linux/shell-script-simple-select-menu#:~:text=We%20should%20use%20the%20select,of%20options%20preceded%20by%20numbers.&text=select%20repeatedly%20reads%20a%20number,NAME%20to%20the%20respective%20text.
