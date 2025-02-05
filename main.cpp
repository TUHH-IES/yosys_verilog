#include "kernel/register.h"

int main()
{
    std::string filename = "dff.v";

    Yosys::log_files.push_back(stdout);
    Yosys::log_error_stderr = true;
    Yosys::yosys_setup();
    Yosys::Design design;
    Yosys::Frontend::frontend_call(&design, NULL, filename, "verilog");

    Yosys::Pass::call(&design, "proc");
    std::stringstream json_representation;
    Yosys::Backend::backend_call(&design, &json_representation, "", "json");

    return 0;
}
