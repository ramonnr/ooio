#include "ooio.hpp"

int main(){

    FSGPIO io {
        IOConfigBuilder()
            .name("9")
            .direction(Direction::OUT)
            .build()
    };

    std::cout << "is ok: " << io.is_ok() << "\n";
    io.value(false);
    std::cout << "value: " << io.value() << "\n";
    io.value(true);
    std::cout << "value: " << io.value() << "\n";

    return 0;
}
