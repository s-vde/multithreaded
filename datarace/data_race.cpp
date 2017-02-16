
#include <thread>

int main()
{
    int x, y, z;
    
    std::thread t1([&y]
    {
        y = 1;                      // datarace: line 16
    });
    
    std::thread t2([&x, &y, &z]
    {
        x = 1;
        z = x + y;                  // datarace: line 10
    });
                   
    t1.join();
    t2.join();
    
    return 0;
}
