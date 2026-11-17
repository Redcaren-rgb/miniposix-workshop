# Workshop assignment: build, verify, and audit MiniPOSIX

You are developing a **small library**, not a POSIX framework. Use OpenCode as a collaborator, but check its suggestions against tests and documentation. The goal is to learn how to verify AI-assisted work in a domain you may not know yet.

## Before coding

Rate your starting familiarity from **0 (none)** to **3 (comfortable)**. Keep the answers in your team's notes; they are for reflection, not grading.

| Topic | 0–3 | One thing you already know or need to look up |
| --- | --- | --- |
| C++ ownership and move semantics |1| |
| POSIX file descriptors and pipes |1| |
| AI coding agents |2| I know how to use AI agents, but I need to verify their suggestions.|

Read [README.md](README.md), run the tool checks, configure `clang22-debug`, and run the smoke and functional tests separately. A smoke pass with functional failures is the intended initial state.

## Audit A: an AI agent in an unfamiliar domain

**Question:** Can your team use an agent to build a small library and verify its behavior without already knowing POSIX?

Implement the contract in the public headers:

- `miniposix::UniqueFd` owns at most one file descriptor. Its default state is invalid. The destructor closes an owned descriptor. `get()` observes it, `release()` transfers the raw descriptor out without closing it, and `reset()` replaces or clears ownership. Moving transfers ownership and invalidates the source. Copying is disabled.
- `miniposix::Pipe::create()` creates one read end and one write end or reports the operating-system error with `std::system_error`. `read_end()` and `write_end()` expose references to their owning `UniqueFd` objects; call `.get()` to pass a raw descriptor to POSIX I/O. The headers define the exact signatures and error behavior.
- Correct behavior includes resource lifetime, not just return values. A test should be able to show that an old descriptor is closed after ownership changes. You may add focused public tests while you work.

Useful terms to look up: `open`, `close`, `pipe`/`pipe2`, `read`, `write`, `errno`, RAII, move constructor, move assignment, and `std::system_error`. Read the relevant documentation before accepting an agent's explanation. The optional I/O helper exercise must distinguish end-of-file, interrupted calls, errors, and short transfers if you choose to attempt it. `fork`, `exec`, signal handling, `poll`, and an event loop are outside the required implementation.

Suggested sequence:

1. Ask OpenCode to explain the contract and propose an implementation and test plan. Make it identify any assumptions about ownership or error paths.
2. Approve commands deliberately. Inspect the diff and explain each change to a teammate.
3. Build, run the smoke test, then run functional tests. For a failure, inspect the real descriptor behavior and revise the code or test as needed.
4. Consult POSIX/C++ documentation to verify at least one claim made by the agent. Record a mistake or an uncertain claim, even if the code ultimately works.
5. Commit, push, open a PR **to `main` in your own fork**, and inspect the Actions result. A green CI result is feedback, not proof of complete correctness.

Keep a short log of your attempts. Add rows as needed:

| Attempt | Failing test or observed behavior | Help needed? | AI mistake or uncertain claim | Evidence and reason for the correction |
| --- | --- | --- | --- | --- |
| 1 | Initial functional tests failed| Yes|Some ownership behavior needed verification | |
| 2 | Tested descriptor ownership after moving| yes| | |

This exercise records what your team observed. It cannot establish that AI replaces domain knowledge without a broader controlled study.

Before moving to Audit B, check that you have a successful build, a passing smoke test, a reasoned explanation of any functional failures, and a branch pushed to your fork with a PR and CI results. A typical bug to investigate is a descriptor that looks correctly stored in the wrapper but remains open after ownership is replaced.

## Audit B: correctness and performance

Once the functional tests pass, use the provided pipe benchmark and CSV analysis described in [README.md](README.md). The benchmark should complete for each buffer size, verify the bytes transferred, and save repeat measurements. It handles short I/O and avoids depending on timing thresholds in CI.

In your team's notes, answer:

1. Which buffer size had the highest median throughput in your run? Was the ordering stable across repeats?
2. What parts of the transfer are included in the timing? Why is this **not** a measurement of the `UniqueFd` wrapper alone?
3. Which observations might change on another machine or under a different system load? If you used `perf stat`, which counters were available and what did they add?
4. What would you change in the experiment before claiming that one variant is generally faster?

If your library is unfinished, use the instructor's reference variant or a supplied CSV for the analysis portion. Report the provenance of the data. Do not invent measurements. You can ask AI to suggest interpretations, but check them against the raw CSV, repeats, and measurement conditions.

## Afternoon exercise: compare AI answers and design a verification workflow

**Timebox: 35 minutes.** Work with one specific POSIX question that matters to your implementation, such as descriptor ownership, end-of-file, or short `read`/`write` calls:

1. **5 minutes:** Ask the same question in two separate agent conversations (or with two available models). Save both answers, compare their checkable claims, and mark one important claim. Note any disagreement; agreement alone is not evidence.
2. **10 minutes:** Find a relevant POSIX/Linux documentation page. Record its URL and what it says about the claim.
3. **10 minutes:** Run a focused test and record the command and observed result. A small standalone test is fine if your library is unfinished. Explain what the test shows in its conditions and what still needs a documented rule.
4. **10 minutes:** Save one small JSON evidence record with the fields `claim`, `source_url`, `test_command`, `observed`, and `verdict`. A team member must set `verdict` to **accept**, **revise**, or **reject** after reviewing both answers, the source, and the test. Note any unresolved uncertainty. Draw a small workflow, or write a short script, that collects the evidence and stops before submitting or updating a PR for this **human decision**. Show where an accepted change goes next (review and CI) and where a rejected or uncertain claim goes for revision.

Bring the two saved answers, documentation link, test result, JSON record, and workflow sketch or script to the final discussion. This is a verification workflow design; it does not require a service or a new C++ API.

### If your team finishes early

- Add one public test for `Pipe` move assignment. Give the destination pipe its own descriptors before the assignment and check with a POSIX call that those old descriptors are closed afterward. Explain why checking only the wrapper values would miss a leak.
- Run the benchmark again with the same transfer settings and at least 10 repeats, saving to a fresh CSV path. Compare the median and spread with your own first run, not a supplied CSV from another machine. List the variables recorded in the CSV and relevant conditions it does not record, such as CPU, system load, OS, and compiler. Explain how these missing features limit any performance claim.

## Finish and discuss

Show the instructor your fork's PR, local test results, attempt log, CSV/plot or supplied-data analysis, and the afternoon evidence record and workflow. Mark what you could not verify. For the final architecture discussion, sketch one possible extension, such as a subprocess or an event-driven interface, and identify which new ownership or error cases it would introduce; no implementation is required.
