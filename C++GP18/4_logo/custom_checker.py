# Start of BODY
'''
TestStruct::
testcase_id                   [int] ID of the test-case
testcase_input_path           [str] File path to test-case input
testcase_output_path          [str] File path to test-case output generated by the problem solver
testcase_expected_output_path [str] File path to test-case expected output to be matched with
testcase_error_path           [str] File path to test-case STDERR
metadata_file_paths           [list<str>] File paths to Question metadata (Extra files usually used for defining traning sets)
submission_code_path          [str] File path to submission source code
submission_language           [str] Language token of submission
testcase_result               [bool] Set to True if test-case output matches test-case expected output. Matching is done line by line
testcase_signal               [int] Exit code of the test-case process
testcase_time                 [float] Time taken by the test-case process in seconds
testcase_memory               [int] Peak memory of the test-case process determined in bytes
data                          [str] <Future use>
ResultStruct::
result      [bool]  Assign test-case result. True determines success. False determines failure
score       [float] Assign test-case score. Normalized between 0 to 1
message     [str] Assign test-case message. This message is visible to the problem solver
'''

def run_custom_checker(t_obj, r_obj):
    import os
    length = os.stat(t_obj.submission_code_path).st_size
    with open(t_obj.testcase_output_path, "r") as fout, open(t_obj.testcase_expected_output_path, "r") as fref:
        r_obj.result = fout.readlines() == fref.readlines()
    r_obj.score = max(0, 2050 - length) / 2050 if r_obj.result else 0;
    r_obj.message = "Success, character count = {0}".format(length) if r_obj.result else "Failure";

# End of BODY
