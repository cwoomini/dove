# The Dove Programming Language

<p align="center">
  <a href="#why">Why</a> |
  <a href="#example">Example</a> |
  <a href="#building-from-source">Building from source</a>
</p>

Status: **Not Ready** (Work-In-Progress)

## Why

I created this project to learn how programming languages are designed, compiled, and executed. This is solely a hobby project, so it is not meant to be production-ready. Dove is inspired by Rust (or at least my understanding of it, since I haven't written much in Rust).

## Example

```
use dove::{std!, rand, rng, fmt};

obj Counter {
  let value: u8;
  const max: u8 = 100;
  const min: u8 = 0;

  func increment() -> bool {
    if value < max {
      value += 1;
      true
    } else { false }
  }

  func decrement() -> bool {
    if value > min {
      value -= 1;
      true
    } else { false }
  }
}

func main() {
  let msg: [ch, ~] = create_msg("Dove");
  println("{}", msg);

  let counter: Counter = { value = 0 };

  for _ in rng::range(0, 100) {
    const action: bool = rand::rand_bool();

    match action {
      is true: counter.increment();
      is false: counter.decrement();
    }

    println("Counter is {}", counter.value);
  }
}

func create_msg(name: const &[ch, ~]) -> [ch, ~] {
  fmt::format("Hello! This is the {} Programming Language", name)
}
```

## Building from source

To build and run tests:

```sh
make
```

All tests in `lib/tests/` will be compiled into `build/bin/`.

To build the release library:

```sh
make release
```

The static library will be generated in `build/`.
