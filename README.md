# Mentor Matching

A C++ implementation of the Gale–Shapley stable matching algorithm for mentor-mentee pairing. Supports partial preference lists, multi-round history (to avoid repeat matches), twin assignments (one mentor receiving two mentees), and compatibility-scored optimization.

In this system, **mentees are proposers** and **mentors are receivers**.

---

## Dependencies

- `g++` with C++17
- Python 3 (for preprocessing only)
- A Python virtual environment at `venv/` (run `python3 -m venv venv && venv/bin/pip install` if missing)

---

## Quickstart

```bash
# 1. Build the binary
make

# 2. Run matching
./stablematching data/round_x_littles.csv data/round_x_bigs.csv
```

---

## Input Format

Both the littles and bigs CSVs use the same format: one row per person, with their name first followed by their ranked preferences (most preferred first).

```
name,first_choice,second_choice,third_choice,...
```

Partial lists are allowed, and someone who submitted no preferences can appear as just their name with no choices.

Names are matched **case-insensitively** and whitespace is stripped.

---

## Preprocessing

If your input is a raw Google Form CSV export, use the preprocessing script to convert it to the expected format.

```bash
python3 src/preprocessing.py <input_form.csv> <output_clean.csv>
```

The script automatically finds:
- The **name column**: any column whose header contains the word "name"
- The **choice columns**: any columns (after the name column) whose headers contain the word "choice"

It lowercases all values and strips whitespace. Run it separately for the mentors' and mentees' rankings.

You can also use the Makefile targets with your form paths:

```bash
make preprocess LITTLES_FORM="data/littles_form.csv" BIGS_FORM="data/bigs_form.csv"
```

This writes cleaned CSVs to `data/round_x_littles.csv` and `data/round_x_bigs.csv` (configurable via `LITTLES_CSV` and `BIGS_CSV` in the Makefile).

---

## Running Matching

```bash
./stablematching <littles_csv> <bigs_csv> [prior_results.csv ...] [--twins big_name ...]
```

### Arguments

| Argument | Description |
|---|---|
| `littles_csv` | Cleaned CSV of littles' preferences |
| `bigs_csv` | Cleaned CSV of bigs' preferences |
| `prior_results.csv ...` | (Optional) One or more result files from previous rounds |
| `--twins big_name ...` | (Optional) Force specific bigs to receive two littles |

### Examples

Basic matching:
```bash
./stablematching data/round_1_littles.csv data/round_1_bigs.csv
```

Accounting for prior round history:
```bash
./stablematching data/round_2_littles.csv data/round_2_bigs.csv data/round_1_results.csv
```

Forcing specific bigs to be twins, plus prior history:
```bash
./stablematching data/round_3_littles.csv data/round_3_bigs.csv \
  data/round_1_results.csv data/round_2_results.csv \
  --twins big1_name big2_name
```

---

## Prior History Format

Result CSVs list each big followed by their little(s) on the same row:

```
big_name,little1_name,little2_name,...
```

Pass as many prior result files as needed; the algorithm accumulates history across all of them.

---

## Output

The program prints each big's assignment with mutual ranking information, followed by the total compatibility score.

```
Receiver: big_name
  little1_name (proposer rank: 1, receiver rank: 6)
  little2_name (proposer rank: 1, receiver rank: 1)

Compatibility score: 8
```

- **proposer rank**: where this big appeared on the little's preference list (1 = top choice). `-1` means the little did not rank this big.
- **receiver rank**: where this little appeared on the big's preference list (1 = top choice). `-1` means the big did not rank this little.

A **lower compatibility score is better**.

---

## How Compatibility Is Measured

The algorithm runs **100 iterations** per capacity layout and keeps the result with the lowest total compatibility score. Each pairing within a match contributes to the score as follows.

### Base score (per little–big pairing)

| Situation | Score |
|---|---|
| Both ranked each other | `(little's rank × 3) + (big's rank × 2)` |
| Only big ranked little | `(big's rank × 2) + 250` |
| Only little ranked big | `(little's rank × 3) + 125` |
| Neither ranked the other (both submitted lists) | `500` |
| Neither ranked the other (mentor submitted no list) | `250` |
| Neither ranked the other (mentee submitted no list) | `125` |
| Neither ranked the other (neither submitted a list) | `0` |

Ranks are 1-indexed (1 = first choice), so a mutual top-choice match scores 5, while low-ranked or unranked pairings score much higher.

Mentee's rank is weighted more heavily (×3 vs ×2) because the algorithm is mentee-optimal — mentees propose, so the stable matching already favors their preferences; the scoring compensates by more aggressively penalizing outcomes where the little got a poor match.

### Penalty adjustments

| Condition | Additional penalty |
|---|---|
| Mentor and mentee were matched in a prior round | `+10,000` |
| Mentor is receiving twins and already has prior match history | `+50 × (number of prior mentees)` |

The previously-matched penalty is intentionally very large to strongly discourage repeat pairings when prior round data is provided. The twin penalty distributes twin assignments toward mentors who have met fewer people overall.

### Capacity optimization

When there are more mentees than mentors, some mentors must receive twins. Unless specific mentors are forced via `--twins`, the algorithm searches **every valid combination** of who gets twins and runs 100 iterations for each, returning the overall best result.

---

## How the Algorithm Works

The core is **Gale–Shapley** (mentee-proposing):

1. All mentees start free and propose to their top-ranked mentor.
2. A mentor tentatively accepts if they have capacity; otherwise they compare the new proposer to their current match and keep the one they prefer.
3. Rejected mentees propose to their next choice. This repeats until everyone is matched.

**Partial lists** are handled by randomly filling in any unranked people at the end of each person's preference list before each iteration. Previously-matched people are placed last in these random fills, reducing (but not eliminating) the chance they appear as a fallback choice. Running 100 iterations with different random fills lets the algorithm find a stable matching that scores well even when some preferences are incomplete.
