package main

import "path"

type IOs struct {
	Cnt int             // IOs referenc count
	Map map[string]bool // abspaths; inputs are false, outputs are true
}

type ProcState struct {
	SysEnter bool           // true on enter to syscall
	Syscall  int            // call number on exit from syscall
	CurDir   string         // working directory
	ExecPath string         // current executable
	NextExec string         // next executable
	FDs      map[int]string // map fds to abspaths

	IOs *IOs
}

type Record struct {
	Command string
	Inputs  []string
	Outputs []string
}

func NewProcState() *ProcState {
	return &ProcState{
		FDs: make(map[int]string),
		IOs: &IOs{1, make(map[string]bool)},
	}
}

func (ps *ProcState) ResetIOs() {
	ps.IOs.Cnt--
	ps.IOs = &IOs{1, make(map[string]bool)}
}

func (ps *ProcState) Abs(p string) string {
	if path.IsAbs(p) {
		return p
	}
	return path.Join(ps.CurDir, p)
}

func (ps *ProcState) Clone() *ProcState {
	newps := NewProcState()
	newps.IOs = ps.IOs // IOs are shared until exec
	ps.IOs.Cnt++
	newps.CurDir = ps.CurDir
	for n, s := range ps.FDs {
		newps.FDs[n] = s
	}
	return newps
}

func (ps *ProcState) Record() Record {
	r := Record{Command: ps.ExecPath}
	for s, output := range ps.IOs.Map {
		if output {
			r.Outputs = append(r.Outputs, s)
		} else {
			r.Inputs = append(r.Inputs, s)
		}
	}
	return r
}
