echo single_threaded
time ./single_threaded > /dev/null
echo multi_threaded
time ./multi_threaded > /dev/null
echo concurrent_callbacks
time ./concurrent_callbacks > /dev/null
echo concurrent_coroutines
time ./concurrent_coroutines > /dev/null