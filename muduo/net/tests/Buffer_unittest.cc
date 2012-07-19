#include <muduo/net/Buffer.h>

//#define BOOST_TEST_MODULE BufferTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using muduo::string;
using muduo::net::Buffer;

BOOST_AUTO_TEST_CASE(testBufferAppendRetrieve)
{
  Buffer buf;
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);

  const string str(200, 'x');
  buf.append(str);
  BOOST_CHECK_EQUAL(buf.readableBytes(), str.size());
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize - str.size());
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);

  const string str2 =  buf.retrieveAsString(50);
  BOOST_CHECK_EQUAL(str2.size(), 50);
  BOOST_CHECK_EQUAL(buf.readableBytes(), str.size() - str2.size());
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize - str.size());
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend + str2.size());

  buf.append(str);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 2*str.size() - str2.size());
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize - 2*str.size());
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend + str2.size());

  const string str3 =  buf.retrieveAllAsString();
  BOOST_CHECK_EQUAL(str3.size(), 350);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);
}

BOOST_AUTO_TEST_CASE(testBufferGrow)
{
  Buffer buf;
  buf.append(string(400, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 400);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-400);

  buf.retrieve(50);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 350);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-400);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend+50);

  buf.append(string(1000, 'z'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 1350);
  BOOST_CHECK_EQUAL(buf.writableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend+50); // FIXME

  buf.retrieveAll();
  BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
  BOOST_CHECK_EQUAL(buf.writableBytes(), 1400); // FIXME
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);
}

BOOST_AUTO_TEST_CASE(testBufferInsideGrow)
{
  Buffer buf;
  buf.append(string(800, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 800);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-800);

  buf.retrieve(500);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 300);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-800);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend+500);

  buf.append(string(300, 'z'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 600);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-600);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);
}

BOOST_AUTO_TEST_CASE(testBufferPrepend)
{
  Buffer buf;
  buf.append(string(200, 'y'));
  BOOST_CHECK_EQUAL(buf.readableBytes(), 200);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-200);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);

  int x = 0;
  buf.prepend(&x, sizeof x);
  BOOST_CHECK_EQUAL(buf.readableBytes(), 204);
  BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-200);
  BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend - 4);
}
