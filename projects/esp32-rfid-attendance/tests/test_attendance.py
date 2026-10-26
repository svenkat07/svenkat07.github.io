import importlib.util
import json
import pathlib
import unittest
root=pathlib.Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('attendance',root/'tools/attendance.py')
mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
class Tests(unittest.TestCase):
 def test_filter_deduplicate_and_label(self):
  e=dict(uptime_ms=0,decision='granted',subject='demo_user_01',simulated=True)
  deny=dict(e,decision='denied')
  out=list(mod.records(map(json.dumps,[e,e,deny]),'test'))
  self.assertEqual(len(out),1);self.assertTrue(out[0][1]['simulated'])
  self.assertEqual(out,list(mod.records([json.dumps(e)],'test')))
 def test_session_separation(self):
  e=json.dumps(dict(uptime_ms=0,decision='granted',subject='demo_user_01',simulated=True))
  self.assertNotEqual(list(mod.records([e],'a'))[0][0],list(mod.records([e],'b'))[0][0])
 def test_bad_session(self):
  with self.assertRaises(ValueError):list(mod.records([],'bad/session'))
if __name__=='__main__':unittest.main()
