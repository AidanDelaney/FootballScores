---
title: STL Algorithms
subtitle: Concepts, Usage, and Best Practices
author: Aidan Delaney <aidan@ontologyengineering.org>
date: February 2018

aspectratio: 169
slideNumber: true
center: false
---

# Introduction {data-background="#CCCCCC" data-transition="fade-in fade-out"}

## <span style="color:#ffffff">About Aidan</span> {data-background='images/royal-pavilion-brighton.jpg' }

<div style="background-color:#333333cc">

* <span style="color:#ffffff">PhD in diagrams that can be used to reason about software constraints.</span>
* <span style="color:#ffffff">Researcher in Visual Languages and Visual Reasoning.</span>
* <span style="color:#ffffff">Shipped code in Haskell, C, Java, Perl, Python, C++, JavaScript & others.</span>
* <span style="color:#ffffff">Director of an Eastbourne not-for-profit [__TechResort__](http://techresort.co.uk/).</span>
* <span style="color:#ffffff">Involved with BCS Computing at School project for a while.</span>

</div>

## Objective

* To examine how `#include <algorithm>` can: <br /><br />

  1. improve efficiency,
  2. improve code correctness, and
  3. improve code clarity.

## Your Skills Audit

* You have experience with STL containers.
* You mostly write C++03 (our current standard).
* You're interested in C++11 improvements.
* You're not interested in move semantics.

## Approach

* Work-along
    - [http://github.com/AidanDelaney/FootballScores](github.com/AidanDelaney/FootballScores).

* Timeboxed Answers (2min)
* Promises

# Recap {data-background="#0075B2" data-transition="fade-in fade-out"}

## Containers I

STL sequence containers

|         | `list`     | `vector`                      | `array` |
|---------|------------|-------------------------------|---------|
| access  | sequential | random                        | random  |
| insert  |   $O(n)/O(1)$   | $O(n)/O(1)$[^vector-insert]   | $O(1)$  |
| find    |   $O(n)$   | $O(n)/O(log n)$[^vector-find] | $O(n)/O(log n)$  |

[^vector-insert]: $O(1)$ only at the back and if we're have previously reserved space.
[^vector-find]: $O(log n)$ if the vector is sorted.

## Containers II

STL associative containers

|        | `map`      | `unordered_map` |
|--------|------------|-----------------|
| access | $O(log n)$ | $O(1)$          |
| insert | $O(log n)$ | $O(1)$          |
| find   | $O(log n)$ | $O(1)/O(n)$[^worst-case] |
  
**bsearch.cpp: But constants matter and size matters!**

[^worst-case]: This is worst case

# What (Usage) {data-background="#00d414" data-transition="fade-in fade-out"}

## Running Example

* Using 2016-2017 season English Championship results from the BBC.

```c++
struct GameResult {
  std::string home_team;
  std::string away_team;
  int home_score;
  int away_score;
};

// C79 -- Store only values and smart pointers in containers.
std::vector<GameResult> results;
```

* We will interrogate the data to generate summary statistics.

## Loop Count

How many games ended in a home team victory?

```c++
int home_won = 0;
for(std::vector<GameResult>::const_iterator it 
      = results.cbegin(); // C15, S26
    it < results.cend(); 
	++it) // C28
  {
    if(it->home_score > it->away_score) {
      ++home_won;
    }
  }
```

## `#include <algorithm>`

  * Provides Algorithms that work on STL containers

```c++
// C25, S39/C87
bool home_win(const GameResult &gr) {
  return gr.home_score > gr.away_score;
}

// S43/C84, S45/C85
int home_won = std::count_if(results.cbegin() // S26
	                         , results.cend()
							 , home_win); // !C88
```

## Our Objectives

Efficiency (Demo: stats_eg.cpp)
: `std::count_if` version is 10-200% faster on my machine.

Correctness
: `std::count_if` avoids common loop mistakes & predicate is easily testable.

Clarity
: `std::count_if` communicates that we are counting based on a predicate.

## Loop Correctness

```c++
int home_won = 0; /* Issue 1 */
for(std::vector<GameResult>::const_iterator it /* Issue 2 */
      = results.cbegin();
    it < results.cend(); /* Issue 3 */
	++it /* Issue 4 */) 
  {
    if(it->home_score > it->away_score) {
      ++home_won; /* Issue 5 */
    }
  }
```

1. Initialisation of count variable is often forgotten.
2. Remember to use `const_iterator` type.
3. Is it `<` or `<=`? Or an accidental `>`?
4. Loop variable updates are often forgotten.
5. Remember to update the count variable.

## `count_if` Correctness

```c++
bool home_win(const GameResult &gr) {
  return gr.home_score > gr.away_score; /* Issue 1 */
}

                                  /* Issue 2 & 3 */
int home_won = std::count_if(results.cbegin()
	                         , results.cend()
							 , home_win);
```

1. We could get the predicate wrong -- but it's now testable.
2. We could mix up `cbegin` and `cend`.
3. We may forget to use `const` iterators.

## Available Algorithms

There are [many, in the `algorithm` reference](http://en.cppreference.com/w/cpp/algorithm) manual

Non-modifying sequence operations[^non-modifying]
: `count`, **`count_if`**, `find`, `find_if`, `search`

Modifying sequence operations
: `copy`, `fill`, `remove`, `remove_if`, **`transform`**

Binary search operations -- sorted ranges
: **`binary_search`**, `upper_bound`, 

Numeric operations
: `accumulate`, `inner_product`, 

[^non-modifying]: May not actually be non-modifying!

## Transform Example

Suppose we wanted the data in the format:

```c++
struct GameSpread {
  std::string winner;
  uint spread
};
```

Given our previous `results` vector we can:

```c++
std::vector<std::shared_ptr<GameSpread>> spread;
spread.reserve(total_games);

std::transform(results.cbegin(), results.cend()
	           , std::back_inserter(spread)
			   , // lambda elided for great good
			   );
```

## Isn't that `for_each`?

* `transform` conveys the semantics that we're mapping one struture to another.
    - no side-effects!
* `for_each` function object may have side-effects (and often does).
* The calls return different things.
* `for_each` applies the function object in sequential order.  `transform` is not required to operate sequentially.

## `binary_search`

Similar, to `transform` the `binary_serarch` algorithm operates over a range:

```c++
std::vector<std::pair<int, int>> zipped_intv;
const std::pair<int, int> eg = mk_pair(EG_TO_FIND);

std::binary_search(items.cbegin(), items.cend(), eg);
```

## Commonalities

From our three examples we can see characteristics shared by `<algorithm>` :

  1. algorithms work on ranges
  2. ranges are provided by iterators
  3. function objects are commonly used

# How (Concepts) {data-background="#c32" data-transition="fade-in fade-out"}

## Iterators

- Generalised pointer.
- All Iterators suppport `++`.
- Iterators come in [several flavours](http://www.cplusplus.com/reference/iterator/):
    * InputIterator -- read-only, supports `==` testing.
	* OutputIterator -- useable as an lvalue.
	* ForwardIterator -- supports `=`.
	* RandomAccessIterator
	* ...
	
- The flavour of iterator is one of the **traits** of an iterator
    * Uses templates and multiple inheritance to constrain iterators at compile time i.e. the type-level.

- E47 Use traits classes for information about types

## Iterator Invalidation

Iterator use introduces a class of error:

```c++
for(auto it = std::lower_bound(vs.begin(), vs.end(), four)
	; it < vs.end() ; ++it) {
    for(auto ut = us.begin()
	  ; ut < us.end(); ++ut) {
      vs.insert(it, *ut);
    }
  }
```

## Insert & Erase

* Inserting a new element invalidates some iterators on:
    - `vector`, `deque`
* Erasing an element invalidates iterators on:
    - `vector`, `deque`
* Erasing invalidates iterators to the erased element on:
    - `list`, `set`, `map`

* While STL alogrithms don't solve iterator invalidation; they simplify the handling of iterators.

## Function Objects

* a.k.a. Functors
* Object on which the `()` operator is defined.
* Allows "partial application" of functions.

```c++
auto brighton_named = std::bind(IsNamedTeam(), brighton, _1);

int brighton_played = std::count_if(results.cbegin(), results.cend(), brighton_named);
```

## Pure Functions

* Pure functions are functions without side effecs.
    - Given the same input, they always return the same output.
    - they do not interact with their environment
	- they do not write to the console, or read a file.
	- they do not modify state.
* Many `<algorithm>`s reqire pure functions. 

## Predicates

* Are a special type of pure functions that take $n$ parameters and return a `bool`:
* `unary_function` and `binary_function` are widely required.
* The following is a functor, is pure and is a predicate:

```c++
struct IsHomeWin : std::unary_function<const GameResult&, bool> {
  bool operator()(const GameResult& gr) const noexcept {
    return gr.home_score > gr.away_score;
  }
};
```

## On E34 & C88

E34 states "Prefer lambdas to `std::bind`"

but C88 states "Compilers have routinely inlined [Function Object] calls since C++'s Bronze Age."

* `std::bind` returns a functor
* replace it with a lambda where the lambda is more readable.
* use it where you need to call an overloaded function.

# Conclusion {data-background="#f08" data-transition="fade-in fade-out"}

## Summary

* We have considered
    - How to **use** STL algorithms
    - Some **best practices** around STL algorithms.
    - Iterator and Function Object as **concepts** used in STL algorithms.

* STL Algorithms improve code quality by:
    1. Being efficient.
    2. Simplifying correctness.
    3. Being clear.

## More Advanced

* Idiomatic use of `<algorithm>`
* Additional algorithms in BSL
    - notably hash functions
	- note `count_if` is defined in `bslstl_algorithmworkaround.h`

## Sources I

Three books provide significant guidance on the use of `algorithm`:

* Scott Meyers, _Effective Modern C++_, O'Reilly, 2014.
* Scott Meyers, _Effective STL: 50 Specific Ways to Improve Your Use of the Standard Template Library_, Addison-Wesley, 2001.
* Herb Sutter & Andrei Alexandrescu, _C++ Coding Standards: 101 Rules, Guidelines, and Best Practices_, Pearson, 2005.

## Sources II

As always, the FAQ is peerless:

* The ISO C++ FAQ: [https://isocpp.org/faq](https://isocpp.org/faq).

## Thank You

* Aidan Delaney <aidan@ontologyengineering.org>
* @aidandelaney

# Bonus Material

## Bjarne Quotes

> By default, use vector when you need a container.

-- Bjarne Stroustrup

> Prefer [STL] algorithms to loops.

-- Bjarne Stroustrup

## Scott Quotes

> When faced with a choice between an STL algorithm or a container member function with the same name, you should prefer the member function.

-- Scott Meyers, Effective STL.

## Demos

1. Iterator Invalidation: `src/i_invalidation.cpp`
2. `count_if` and comparison with loop: `src/stats_eg.cpp`
3. Storing pointers in a container: `src/fail_delete_eg.cpp`
4. Binary search on structures: `src/bsearch.cpp`
5. Using `std::bind`: `src/bind_eg.cpp`
6. Determining strcture size: `src/stlsizeof.cpp`

## Idomatic Usage

Erase-Remove (E32)
: Actually get rid of the elements you've removed.
    - `v.erase(std::remove_if(v.begin(), v.end(), IsOdd), v.end());`

Swap-to-fit (E17)
: Shrink a container to [reclaim space](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Shrink-to-fit).

Copy-and-swap
: Implementing `operator=` in a [_move-friendly_ way](https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom).

Copy-print
: Use `std::copy` to copy elements to an output stream such as `std::cout`.

## `std::bind`

```c++
std::string brighton("Brighton");
auto brighton_pred = std::bind(IsNamedTeam(), brighton, _1);
auto home_pred = std::bind(IsHomeWin(), _1);

auto brighton_home_win_pred = std::bind(std::logical_and<bool>(), brighton_pred, home_pred);
 ```

## equvalence `!=` equality

Beware the difference between 

* equality i.e. "operator==", and
* equivalence as defined by `less` on partially ordered structures.
