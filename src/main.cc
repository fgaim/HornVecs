/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <iostream>
#include <queue>
#include <iomanip>
#include "hornvecs.h"
#include "args.h"

using namespace hornvecs;

void printUsage() {
  std::cerr
    << "usage: hornvecs <command> <args>\n\n"
    << "The commands supported by hornvecs are:\n\n"
    << "  supervised              train a supervised classifier\n"
    << "  quantize                quantize a model to reduce the memory usage\n"
    << "  test                    evaluate a supervised classifier\n"
    << "  predict                 predict most likely labels\n"
    << "  predict-prob            predict most likely labels with probabilities\n"
    << "  skipgram                train a skipgram model\n"
    << "  cbow                    train a cbow model\n"
    << "  print-word-vectors      print word vectors given a trained model\n"
    << "  print-sentence-vectors  print sentence vectors given a trained model\n"
    << "  print-ngrams            print ngrams given a trained model and word\n"
    << "  nn                      query for nearest neighbors\n"
    << "  analogies               query for analogies\n"
    << "  dump                    dump arguments,dictionary,input/output vectors\n"
    << std::endl;
}

void printQuantizeUsage() {
  std::cerr
    << "usage: hornvecs quantize <args>"
    << std::endl;
}

void printTestUsage() {
  std::cerr
    << "usage: hornvecs test <model> <test-data> [<k>] [<th>]\n\n"
    << "  <model>      model filename\n"
    << "  <test-data>  test data filename (if -, read from stdin)\n"
    << "  <k>          (optional; 1 by default) predict top k labels\n"
    << "  <th>         (optional; 0.0 by default) probability threshold\n"
    << std::endl;
}

void printPredictUsage() {
  std::cerr
    << "usage: hornvecs predict[-prob] <model> <test-data> [<k>] [<th>]\n\n"
    << "  <model>      model filename\n"
    << "  <test-data>  test data filename (if -, read from stdin)\n"
    << "  <k>          (optional; 1 by default) predict top k labels\n"
    << "  <th>         (optional; 0.0 by default) probability threshold\n"
    << std::endl;
}

void printPrintWordVectorsUsage() {
  std::cerr
    << "usage: hornvecs print-word-vectors <model>\n\n"
    << "  <model>      model filename\n"
    << std::endl;
}

void printPrintSentenceVectorsUsage() {
  std::cerr
    << "usage: hornvecs print-sentence-vectors <model>\n\n"
    << "  <model>      model filename\n"
    << std::endl;
}

void printPrintNgramsUsage() {
  std::cerr
    << "usage: hornvecs print-ngrams <model> <word>\n\n"
    << "  <model>      model filename\n"
    << "  <word>       word to print\n"
    << std::endl;
}

void quantize(const std::vector<std::string>& args) {
  Args a = Args();
  if (args.size() < 3) {
    printQuantizeUsage();
    a.printHelp();
    exit(EXIT_FAILURE);
  }
  a.parseArgs(args);
  HornVecs hornvecs;
  // parseArgs checks if a->output is given.
  hornvecs.loadModel(a.output + ".bin");
  hornvecs.quantize(a);
  hornvecs.saveModel();
  exit(0);
}

void printNNUsage() {
  std::cout
    << "usage: hornvecs nn <model> <k>\n\n"
    << "  <model>      model filename\n"
    << "  <k>          (optional; 10 by default) predict top k labels\n"
    << std::endl;
}

void printAnalogiesUsage() {
  std::cout
    << "usage: hornvecs analogies <model> <k>\n\n"
    << "  <model>      model filename\n"
    << "  <k>          (optional; 10 by default) predict top k labels\n"
    << std::endl;
}

void printDumpUsage() {
  std::cout
    << "usage: hornvecs dump <model> <option>\n\n"
    << "  <model>      model filename\n"
    << "  <option>     option from args,dict,input,output"
    << std::endl;
}

void test(const std::vector<std::string>& args) {
  if (args.size() < 4 || args.size() > 6) {
    printTestUsage();
    exit(EXIT_FAILURE);
  }
  int32_t k = 1;
  real threshold = 0.0;
  if (args.size() > 4) {
    k = std::stoi(args[4]);
    if (args.size() == 6) {
      threshold = std::stof(args[5]);
    }
  }

  HornVecs hornvecs;
  hornvecs.loadModel(args[2]);

  std::tuple<int64_t, double, double> result;
  std::string infile = args[3];
  if (infile == "-") {
    result = hornvecs.test(std::cin, k, threshold);
  } else {
    std::ifstream ifs(infile);
    if (!ifs.is_open()) {
      std::cerr << "Test file cannot be opened!" << std::endl;
      exit(EXIT_FAILURE);
    }
    result = hornvecs.test(ifs, k, threshold);
    ifs.close();
  }
  std::cout << "N" << "\t" << std::get<0>(result) << std::endl;
  std::cout << std::setprecision(3);
  std::cout << "P@" << k << "\t" << std::get<1>(result) << std::endl;
  std::cout << "R@" << k << "\t" << std::get<2>(result) << std::endl;
  std::cerr << "Number of examples: " << std::get<0>(result) << std::endl;
}

void predict(const std::vector<std::string>& args) {
  if (args.size() < 4 || args.size() > 6) {
    printPredictUsage();
    exit(EXIT_FAILURE);
  }
  int32_t k = 1;
  real threshold = 0.0;
  if (args.size() > 4) {
    k = std::stoi(args[4]);
    if (args.size() == 6) {
      threshold = std::stof(args[5]);
    }
  }

  bool print_prob = args[1] == "predict-prob";
  HornVecs hornvecs;
  hornvecs.loadModel(std::string(args[2]));

  std::string infile(args[3]);
  if (infile == "-") {
    hornvecs.predict(std::cin, k, print_prob, threshold);
  } else {
    std::ifstream ifs(infile);
    if (!ifs.is_open()) {
      std::cerr << "Input file cannot be opened!" << std::endl;
      exit(EXIT_FAILURE);
    }
    hornvecs.predict(ifs, k, print_prob, threshold);
    ifs.close();
  }

  exit(0);
}

void printWordVectors(const std::vector<std::string> args) {
  if (args.size() != 3) {
    printPrintWordVectorsUsage();
    exit(EXIT_FAILURE);
  }
  HornVecs hornvecs;
  hornvecs.loadModel(std::string(args[2]));
  std::string word;
  Vector vec(hornvecs.getDimension());
  while (std::cin >> word) {
    hornvecs.getWordVector(vec, word);
    std::cout << word << " " << vec << std::endl;
  }
  exit(0);
}

void printSentenceVectors(const std::vector<std::string> args) {
  if (args.size() != 3) {
    printPrintSentenceVectorsUsage();
    exit(EXIT_FAILURE);
  }
  HornVecs hornvecs;
  hornvecs.loadModel(std::string(args[2]));
  Vector svec(hornvecs.getDimension());
  while (std::cin.peek() != EOF) {
    hornvecs.getSentenceVector(std::cin, svec);
    // Don't print sentence
    std::cout << svec << std::endl;
  }
  exit(0);
}

void printNgrams(const std::vector<std::string> args) {
  if (args.size() != 4) {
    printPrintNgramsUsage();
    exit(EXIT_FAILURE);
  }
  HornVecs hornvecs;
  hornvecs.loadModel(std::string(args[2]));
  hornvecs.ngramVectors(std::string(args[3]));
  exit(0);
}

void nn(const std::vector<std::string> args) {
  int32_t k;
  if (args.size() == 3) {
    k = 10;
  } else if (args.size() == 4) {
    k = std::stoi(args[3]);
  } else {
    printNNUsage();
    exit(EXIT_FAILURE);
  }
  HornVecs hornvecs;
  hornvecs.loadModel(std::string(args[2]));
  std::string queryWord;
  std::shared_ptr<const Dictionary> dict = hornvecs.getDictionary();
  Vector queryVec(hornvecs.getDimension());
  Matrix wordVectors(dict->nwords(), hornvecs.getDimension());
  std::cerr << "Pre-computing word vectors...";
  hornvecs.precomputeWordVectors(wordVectors);
  std::cerr << " done." << std::endl;
  std::set<std::string> banSet;
  std::cout << "Query word? ";
  std::vector<std::pair<real, std::string>> results;
  while (std::cin >> queryWord) {
    banSet.clear();
    banSet.insert(queryWord);
    hornvecs.getWordVector(queryVec, queryWord);
    hornvecs.findNN(wordVectors, queryVec, k, banSet, results);
    for (auto& pair : results) {
      std::cout << pair.second << " " << pair.first << std::endl;
    }
    std::cout << "Query word? ";
  }
  exit(0);
}

void analogies(const std::vector<std::string> args) {
  int32_t k;
  if (args.size() == 3) {
    k = 10;
  } else if (args.size() == 4) {
    k = std::stoi(args[3]);
  } else {
    printAnalogiesUsage();
    exit(EXIT_FAILURE);
  }
  HornVecs hornvecs;
  hornvecs.loadModel(std::string(args[2]));
  hornvecs.analogies(k);
  exit(0);
}

void train(const std::vector<std::string> args) {
  Args a = Args();
  a.parseArgs(args);
  HornVecs hornvecs;
  hornvecs.train(a);
  hornvecs.saveModel();
  hornvecs.saveVectors();
  if (a.saveOutput) {
    hornvecs.saveOutput();
  }
}

void dump(const std::vector<std::string>& args) {
  if (args.size() < 4) {
    printDumpUsage();
    exit(EXIT_FAILURE);
  }

  std::string modelPath = args[2];
  std::string option = args[3];

  HornVecs hornvecs;
  hornvecs.loadModel(modelPath);
  if (option == "args") {
    hornvecs.getArgs().dump(std::cout);
  } else if (option == "dict") {
    hornvecs.getDictionary()->dump(std::cout);
  } else if (option == "input") {
    if (hornvecs.isQuant()) {
      std::cerr << "Not supported for quantized models." << std::endl;
    } else {
      hornvecs.getInputMatrix()->dump(std::cout);
    }
  } else if (option == "output") {
    if (hornvecs.isQuant()) {
      std::cerr << "Not supported for quantized models." << std::endl;
    } else {
      hornvecs.getOutputMatrix()->dump(std::cout);
    }
  } else {
    printDumpUsage();
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv) {
  std::vector<std::string> args(argv, argv + argc);
  if (args.size() < 2) {
    printUsage();
    exit(EXIT_FAILURE);
  }
  std::string command(args[1]);
  if (command == "skipgram" || command == "cbow" || command == "supervised") {
    train(args);
  } else if (command == "test") {
    test(args);
  } else if (command == "quantize") {
    quantize(args);
  } else if (command == "print-word-vectors") {
    printWordVectors(args);
  } else if (command == "print-sentence-vectors") {
    printSentenceVectors(args);
  } else if (command == "print-ngrams") {
    printNgrams(args);
  } else if (command == "nn") {
    nn(args);
  } else if (command == "analogies") {
    analogies(args);
  } else if (command == "predict" || command == "predict-prob") {
    predict(args);
  } else if (command == "dump") {
    dump(args);
  } else {
    printUsage();
    exit(EXIT_FAILURE);
  }
  return 0;
}
