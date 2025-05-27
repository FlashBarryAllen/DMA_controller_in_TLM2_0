#include "CPU.h"
#include "DMA.h"
#include "Memory.h"
#define DEBUGMODE 1

using namespace sc_core;
using namespace sc_dt;
using namespace std;

int sc_main(int argc, char* argv[]) 
{	
	sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",sc_core::SC_DO_NOTHING );
	//testbench
	sc_clock clock("clock", 10, SC_NS); // timer clock
	sc_signal<bool> reset;				// rst
	sc_signal<bool> clear;
	sc_signal<bool> interrupt;
	//DMA
	DMA	dmac("dmac");
	dmac.clock(clock);
	dmac.reset(reset);
	dmac.clear(clear);
	dmac.interrupt(interrupt);
	//CPU
	CPU	cpu("cpu");
	cpu.clock(clock);
	cpu.reset(reset);
	cpu.socket_m(dmac.socket_s);
	cpu.clear(clear);
	cpu.interrupt(interrupt);
	//memory
	Memory hostmem("hostmem");
	Memory devmem("devmem");
	dmac.socket_host_m(hostmem.socket_s);
	dmac.socket_dev_m(devmem.socket_s);
	//VCD	
	sc_trace_file *tf = sc_create_vcd_trace_file("RESULT");	
	sc_trace(tf, clock, "clock");
	sc_trace(tf, reset, "reset");
	sc_trace(tf, clear, "clear");
	sc_trace(tf, interrupt, "interrupt");
	
	sc_trace(tf, dmac.R_SOURCE, "SOURCE");
	sc_trace(tf, dmac.R_TARGET, "TARGET");
	sc_trace(tf, dmac.R_SIZE, "SIZE");
	sc_trace(tf, dmac.R_DIR, "DIR");
	sc_trace(tf, dmac.R_START, "START");
	sc_trace(tf, dmac.slave.data, "dmac_slave_data");
	for (int i = 0; i < 8; ++i) {
		sc_trace(tf, hostmem.mem[i], ("hostmem(" + std::to_string(i) + ")").c_str());
		sc_trace(tf, devmem.mem[i], ("devmem(" + std::to_string(i) + ")").c_str());
	}
	//simulate
	reset.write(0);
	sc_start(20,SC_NS);
	#ifdef DEBUGMODE
	cout << "data in Memory :\n";
	for(int i = 0; i < 8; i++)
	{
		cout << std::dec << "hostmem[" << i << "]: " << std::hex << hostmem.mem[i] << "\t";
		cout << std::dec << "devmem[" << i << "]: " << std::hex <<  devmem.mem[i] << endl;
	}
	#endif
	reset.write(1);
	sc_start(300,SC_NS);
	#ifdef DEBUGMODE
	cout << "data in Memory (After DMA):\n";
	for(int i = 0; i < 8; i++)
	{
		cout << std::dec << "hostmem[" << i << "]: " << std::hex << hostmem.mem[i] << "\t";
		cout << std::dec << "devmem[" << i << "]: " << std::hex << devmem.mem[i] << endl;
	}
	#endif
	
	return(0);
}

