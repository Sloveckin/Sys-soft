import os

def compare_files(test: str) -> bool:

  dir_name = os.path.dirname(__file__)

  dgml_path = os.path.join(dir_name, "dgmls", f"{test}.dgml")
  expected_path = os.path.join(dir_name, "expected", f"{test}.dgml")

  try:
    with open(dgml_path, 'r') as dgml_file:
        dgml_content = dgml_file.read()

        with open(expected_path, 'r') as expected_file:
          expected_content = expected_file.read()

          return dgml_content == expected_content

  except Exception as e:
    print(f"An error occurred: {e}")

def test_1():
  assert compare_files("test2")

def test_2():
  assert compare_files("test2")

def test_3():
  assert compare_files("test3")

def test_4():
  assert compare_files("test4")
