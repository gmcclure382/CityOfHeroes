#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptlib.h>
#include <rng.h>
#include <osrng.h>
#include <dh.h>
#include <hmac.h>
#include <md5.h>
#include <adler32.h>
#include <sha.h>
#include <rsa.h>
#include <filters.h>
#include <files.h>
#include <modes.h>
#include <hex.h>
#include <aes.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace CryptoPP;
using namespace CryptoPP::Weak1;

// Existing UtilitiesLib DH group and serialized precomputation fixtures.
static const byte dh_hex_params[] =
	"3046024100889C7197E0B88D04A2CF46FD125FF29CB80F1FC427107B88F5731A9609"
	"44B55949DFF8ACF6B835DA411129A4922E3F5C6C4D2F12310C5AFF3EF8CA401FA8F6"
	"1F020102";
static const byte dh_hex_precomputation[] =
	"30820429020101020201000240662AAB385DD658F217922B08C8E02829D7D2A0B38A"
	"CE8D651FA6B03DE431DFF4226015F91BD75E713CCC8312497541EABB1872C96CDAEF"
	"024315A13FA1051DA3024024C4E4065E3CD40F564363DE3C64FC421438100A371CDB"
	"846878272E5B9A12653E41B79A3B3097A0C81F2A93D9EB7C6624824EE2A528F42338"
	"B2302786B34B5802403D5713694413755B4F82C7E7E54240A8E057C184560A81E524"
	"C30B985949FF6BAEEF70EAC9DB4647EB1264E27DE79635EF053EF82FB9C9FB1AC117"
	"3F3183D381024019B94BB70B1FA18D418E004AADFD1E1F9F38F50DA55F46FF54184A"
	"D6931DBE8924E8F6BFD53EC3EABF1A373D827C6912711A85A25CDC1B9A17E8A7FD16"
	"7E977F024029864CDC5A9AE82D6297069F4736447481DA4B4F1284665EBBBA203C64"
	"2454999DD7967DBB5AC09C8470B00D31D23C212674E009314D5501B41011A58CB5A3"
	"5D0240633E2A5E02F48C834C8238A1A34B2AE6765108F663C15D4CE1487E28113CA2"
	"620C3A3146BF71CE986BF9D6E39F83BE989974ECD428672B54A0BBDF8D71F8776302"
	"4037FE969553D38D4CC670729C2EFBE753CD869492EFE818238ED042475E7A13736C"
	"08477E14E36A9B13578E55AEA6FF8C41145FB1BB37BCB5637ED65753DEFB75024046"
	"34545BE90D92982D75651685694E5D2BFA493632C9C13FD791843E377150F2C8F6F4"
	"915E2348449A923BFF7258666BE268497AF4760E50D26960D0FE60DDD50240501D79"
	"C0B8148388A43739C5849E658BB1E110DF3B41377513D9FCF07C8FD909AB5B06DEAC"
	"A39114BCBF7E1A1568B641FF20F74B2648835BED2181B43F89F2B802404EB220B3FC"
	"735882C3F595359F6CF25F0514DAA605959FB5592861CA496B535DB13AB2703D03AE"
	"8073F29C96F5155EC6634BEA7F867FD363D034D1213C40BFAB02406D28FC89890C1B"
	"759F8587C22CCA4B1BD48FCC07D6E675EF8CF384123F5E474C91FEA753A98727708E"
	"0FF69C020DB19D4F7C343C6DD95CE4F99747FD5FD1B321024027FAEF5C4118E06229"
	"C0D0990674FDA940FD635D0F1A720E5E9DFC33EFF94B37EFDC0493C26EB5E8E68F7B"
	"D50B6A3252F55EC2AA0C7CAE273FCC1B7F5E47C99A02400BEFA03E8946A44BDF1320"
	"85DC73A211F4216FA69E28C6BA1204CB0F18FA03997614614FBF1683736A5CE67695"
	"0350DB6F67158A45B6A17B6BE421F1E8BFF3F102410085DFAAAD6FDF9E4D9F5A499E"
	"E0432998E1426A259EB263F4DF4837DE11BC33554AF5EA9CDAE4544750C3379BF959"
	"DE708250BBABCCB400E730C7A7964FE5C83902403A8F973428A8F9974494EA33AADF"
	"50993B7A77E1057AE673039F111806DCD3884D10AA68F98028188774216DD9C6C513"
	"E00420BA9D5760AAC6A9A8CB35F782E20241008012B503338A7D80585E74FFF9937F"
	"C4D84AF2918DF72927F6C79CECC04443D80CF0CFF4DF0C8E2E42E0A26506841DA4E2"
	"B0B9AA33D9F33A06F1899157538295";


static void require(bool value, const char* what)
{
	if (!value) throw std::runtime_error(what);
}
// Compare deterministic outputs with the previously vendored Crypto++ snapshot.
static void report(const char* name, const std::vector<byte>& data,
	const char* expected)
{
	SHA256 hash;
	byte digest[SHA256::DIGESTSIZE];
	hash.CalculateDigest(digest, data.data(), data.size());
	std::string encoded;
	StringSource source(digest, sizeof digest, true,
		new HexEncoder(new StringSink(encoded)));
	require(encoded == expected, name);
	std::cout << name << ": " << encoded << '\n';
}
static void append(std::vector<byte>& dst, const std::vector<byte>& src)
{
	dst.insert(dst.end(), src.begin(), src.end());
}
static void load(DH& dh, bool precompute)
{
	HexDecoder params;
	params.PutMessageEnd(dh_hex_params, sizeof dh_hex_params - 1);
	dh.AccessGroupParameters().BERDecode(params);
	if (precompute) {
		HexDecoder precomp;
		precomp.PutMessageEnd(dh_hex_precomputation,
			sizeof dh_hex_precomputation - 1);
		dh.AccessMaterial().LoadPrecomputation(precomp);
	}
}
static void test_rng()
{
	std::vector<byte> all;
	const word32 seeds[] = {0, 1, 0x2f8306af, 0xffffffff};
	for (word32 seed : seeds) {
		LC_RNG rng(seed);
		std::vector<byte> data(1024);
		rng.GenerateBlock(data.data(), data.size());
		append(all, data);
	}
	report("LC_RNG", all,
		"FCF4079C574EB875C17B6E13E0FE632E"
		"0A7456A7B26B05532B709B266919F2BD");
}
static void test_dh()
{
	HexDecoder params;
	params.PutMessageEnd(dh_hex_params, sizeof dh_hex_params - 1);
	DH dh(params);
	HexDecoder precomp;
	precomp.PutMessageEnd(dh_hex_precomputation,
		sizeof dh_hex_precomputation - 1);
	dh.AccessMaterial().LoadPrecomputation(precomp);
	DH plain;
	load(plain, false);
	require(dh.PrivateKeyLength() == 64, "private key length");
	require(dh.PublicKeyLength() == 64, "public key length");
	require(dh.AgreedValueLength() == 64, "shared key length");
	std::vector<byte> all;
	LC_RNG rng(0x2f8306af);
	for (unsigned i = 0; i < 32; ++i) {
		std::vector<byte> a(64), b(64), ap(64), bp(64);
		std::vector<byte> as(64), bs(64);
		dh.GenerateKeyPair(rng, a.data(), ap.data());
		plain.GenerateKeyPair(rng, b.data(), bp.data());
		require(dh.Agree(as.data(), a.data(), bp.data(), true), "DH A");
		require(plain.Agree(bs.data(), b.data(), ap.data(), true),
			"DH B");
		require(as == bs, "DH secret mismatch");
		std::vector<byte> plain_public(64);
		plain.GeneratePublicKey(rng, a.data(), plain_public.data());
		require(ap == plain_public, "precomputation mismatch");
		append(all, a); append(all, b); append(all, ap);
		append(all, bp); append(all, as);
	}
	std::vector<byte> invalid(64, 0), private_key(64, 0);
	std::vector<byte> result(64);
	private_key.back() = 2;
	require(!dh.Agree(result.data(), private_key.data(),
		invalid.data(), true),
		"invalid DH public key accepted");
	report("DH", all,
		"62FED65EB0A0E55888A9E705564450AD"
		"924561FCEEDCF04E8A87CFE8E609BE96");
	ByteQueue serialized;
	dh.GetGroupParameters().DEREncode(serialized);
	std::vector<byte> der(serialized.CurrentSize());
	serialized.Get(der.data(), der.size());
	report("DH_PARAMS", der,
		"917C1DC9D0B5B941BE366325F11AF9D0"
		"67E6F448B703544C625A7978089FD162");
	ByteQueue table;
	dh.GetMaterial().SavePrecomputation(table);
	std::vector<byte> saved(table.CurrentSize());
	table.Get(saved.data(), saved.size());
	report("DH_PRECOMPUTATION", saved,
		"4566E9D4DDC5DEA28BF2F687C70F7B21"
		"69E26FD1E1E94D3221E2516FFB3AD850");
}
static void test_hashes()
{
	const byte message[] = "this is a test";
	std::vector<byte> all;
	MD5 md5;
	Adler32 adler;
	SHA512 sha;
	const byte key[] = "fixed compatibility test key";
	HMAC<SHA1> hmac(key, sizeof key - 1);
	for (unsigned i = 0; i < 32; ++i) {
		std::vector<byte> md(16), ad(4), sh(64), hm(20);
		md5.Update(message, 4);
		md5.Update(message + 4, sizeof message - 5);
		md5.Final(md.data());
		adler.Update(message, sizeof message - 1);
		adler.Restart();
		adler.Update(message, sizeof message - 1);
		adler.Final(ad.data());
		const byte known_md5[] = {
			0x54, 0xb0, 0xc5, 0x8c, 0x7c, 0xe9, 0xf2, 0xa8,
			0xb5, 0x51, 0x35, 0x11, 0x02, 0xee, 0x09, 0x38
		};
		const byte known_adler[] = {0x26,0x33,0x05,0x16};
		require(!std::memcmp(md.data(), known_md5, 16), "MD5 vector");
		require(!std::memcmp(ad.data(), known_adler, 4),
			"Adler vector");
		sha.Update(message, sizeof message - 1); sha.Final(sh.data());
		hmac.Update(message, sizeof message - 1); hmac.Final(hm.data());
		append(all, md); append(all, ad);
		append(all, sh); append(all, hm);
	}
	report("HASHES", all,
		"D20B0F31DC0CD98EFB33A975794EB039"
		"68E8994D9134D043A7E38E97DAFDAF97");
}
static void test_cfb()
{
	LC_RNG rng(0x2f8306af);
	SecByteBlock key(Rijndael::DEFAULT_KEYLENGTH);
	byte iv[Rijndael::BLOCKSIZE];
	rng.GenerateBlock(key, key.size());
	rng.GenerateBlock(iv, sizeof iv);
	RijndaelEncryption cipher(key, key.size());
	RijndaelDecryption unused_decryptor(key, key.size());
	std::vector<byte> all;
	for (unsigned size : {0, 1, 15, 16, 17, 31, 32, 129}) {
		std::vector<byte> plain(size), encrypted(size), decrypted(size);
		for (unsigned i = 0; i < size; ++i) plain[i] = byte(i);
		CFB_Mode_ExternalCipher::Encryption enc(cipher, iv);
		enc.ProcessString(encrypted.data(), plain.data(), size);
		CFB_Mode_ExternalCipher::Decryption dec(cipher, iv);
		dec.ProcessString(decrypted.data(), encrypted.data(), size);
		require(decrypted == plain, "CFB out-of-place roundtrip");
		std::vector<byte> inplace = plain;
		CFB_Mode_ExternalCipher::Encryption enc2(cipher, iv);
		enc2.ProcessString(inplace.data(), inplace.data(), size);
		require(inplace == encrypted, "CFB in-place ciphertext");
		CFB_Mode_ExternalCipher::Decryption dec2(cipher, iv);
		dec2.ProcessString(inplace.data(), inplace.data(), size);
		require(inplace == plain, "CFB in-place roundtrip");
		append(all, encrypted);
	}
	report("AES_CFB", all,
		"D534BDA69ACE68E8B37CA52E0C733AE0"
		"870AD6E4DAA74050D49517FAF731198B");
}
static void test_rsa(const char* fixture)
{
	FileSource source(fixture, true, new HexDecoder);
	RSA::PrivateKey private_key;
	private_key.Load(source);
	RSAFunction public_key;
	public_key.Initialize(private_key.GetModulus(),
		private_key.GetPublicExponent());
	RSAES_OAEP_SHA_Encryptor enc(public_key);
	RSAES_OAEP_SHA_Decryptor dec(private_key);
	LC_RNG rng(0x2f8306af);
	std::vector<byte> all;
	const unsigned maximum = enc.FixedMaxPlaintextLength();
	for (unsigned size : {1U, maximum - 1, maximum}) {
		std::vector<byte> plain(size);
		std::vector<byte> encrypted(enc.FixedCiphertextLength());
		std::vector<byte> decrypted(
			dec.MaxPlaintextLength(encrypted.size()));
		for (unsigned i = 0; i < size; ++i) plain[i] = byte(i);
		enc.Encrypt(rng, plain.data(), plain.size(),
			encrypted.data());
		DecodingResult result = dec.Decrypt(rng, encrypted.data(),
			encrypted.size(), decrypted.data());
		require(result.isValidCoding &&
			result.messageLength == plain.size(),
			"RSA OAEP result");
		decrypted.resize(result.messageLength);
		require(decrypted == plain, "RSA OAEP plaintext");
		append(all, encrypted);
	}
	report("RSA_OAEP_SHA", all,
		"D656ED820067D693C1030BD211C31402"
		"C5A01ADF845781287E718C47AF483326");
}
int main(int argc, char** argv)
{
	try {
		require(argc == 2, "pass RSA fixture path");
		// Exercise the Windows RNG provider and its transitive linkage.
		AutoSeededRandomPool os_rng;
		byte random[32];
		os_rng.GenerateBlock(random, sizeof random);
		test_rng(); test_dh(); test_hashes();
		test_cfb(); test_rsa(argv[1]);
		std::cout << "PASS\n";
	} catch (const std::exception& error) {
		std::cerr << "FAIL: " << error.what() << '\n';
		return 1;
	}
}
