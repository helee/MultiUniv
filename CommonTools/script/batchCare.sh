if [[ $HOSTNAME == *"cms.snu.ac.kr"* ]]; then
  qstat -u "*"
elif [[ $HOSTNAME == *"cms.snu.ac.kr"* ]]; then
  qstat
elif [[ $HOSTNAME == *"ui"*".sdfarm.kr"* ]]; then
  condor_q
  condor_q -hold
else
  echo "Not prepared at " $HOSTNAME
fi
  

