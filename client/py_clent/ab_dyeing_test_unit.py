#!/usr/bin/python
# encoding: utf-8

import unittest

class TestStringMethods(unittest.TestCase):

    def setUp(self):
        print "in set_up"

    def tearDown(self):
        print "in tear_down"

    def test_upper(self):
        self.assertEqual('foo'.upper(), 'FOO')

    def test_isupper(self):
        self.assertTrue('FOO'.isupper())
        self.assertFalse('Foo'.isupper())

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        with self.assertRaises(TypeError):
             s.split(2)

class TestCalcMethods(unittest.TestCase):

    def test_add(self):
        s = 46 + 46 
        self.assertEqual(s, 92),

if __name__ == '__main__':
    unittest.main()
