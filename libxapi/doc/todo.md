* use spinlock to assign per-thread memblks from global storage
* release memblks in mm_teardown
* implement a mode where file/frame/func is not stored directly, but recovered from addresses using libdwarf
