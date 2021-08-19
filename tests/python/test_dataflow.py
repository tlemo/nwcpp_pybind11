
import unittest
import nwcpp


class VariablesTestCase(unittest.TestCase):
    def test_variable_declarations(self):
        dag = nwcpp.Dataflow()
        a = dag.declare_variable('a')
        with self.assertRaises(RuntimeError):
            # duplicate name
            dag.declare_variable('a')
        b = dag.declare_variable(name='b')

    def test_variables_list(self):
        dag = nwcpp.Dataflow()
        c = dag.declare_variable('c')
        b = dag.declare_variable('b')
        a = dag.declare_variable('a')
        self.assertEqual(dag.variables, [a, b, c])

    def test_variable_lookup(self):
        dag = nwcpp.Dataflow()
        c = dag.declare_variable('c')
        b = dag.declare_variable('b')
        a = dag.declare_variable('a')
        self.assertIsNone(dag.lookup_variable(name='B'))
        self.assertIsNone(dag.lookup_variable(name='aa'))
        for v in dag.variables:
            self.assertEqual(dag.lookup_variable(name=v.name), v)


class OperationsTestCase(unittest.TestCase):
    def test_mixing_dags(self):
        dag_1 = nwcpp.Dataflow()
        dag_2 = nwcpp.Dataflow()
        a_1 = dag_1.declare_variable('a')
        a_2 = dag_2.declare_variable('a')
        sum_1 = dag_1.create_binary_op('+', a_1, a_1)
        sum_2 = dag_2.create_binary_op('+', a_2, a_2)
        with self.assertRaises(RuntimeError):
            dag_1.create_binary_op('+', a_1, a_2)
        with self.assertRaises(RuntimeError):
            dag_2.create_binary_op('+', a_1, a_2)
        with self.assertRaises(RuntimeError):
            dag_1.create_binary_op('+', sum_1, sum_2)
        with self.assertRaises(RuntimeError):
            dag_2.create_binary_op('+', sum_1, sum_2)

    def test_binary_operations(self):
        dag = nwcpp.Dataflow()
        a = dag.declare_variable('a')
        b = dag.declare_variable('b')

        # test all supported forms
        div_1 = dag.create_binary_op('/', a, b)
        div_2 = dag.div(a, b)
        div_3 = a / b

        with self.assertRaises(RuntimeError):
            div_1.eval()

        a.assign(8)
        b.assign(4)

        self.assertEqual(div_1.eval(), 2)
        self.assertEqual(div_2.eval(), 2)
        self.assertEqual(div_3.eval(), 2)

    def test_operator_overloading(self):
        dag = nwcpp.Dataflow()
        a = dag.declare_variable('a')
        b = dag.declare_variable('b')
        c = dag.declare_variable('c')
        result = a + b * c
        a.assign(1)
        b.assign(2)
        c.assign(3)
        self.assertEqual(result.eval(), 7)


if __name__ == '__main__':
    unittest.main()
