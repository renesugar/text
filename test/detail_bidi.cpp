#include <boost/text/bidirectional.hpp>

#include <gtest/gtest.h>


using namespace boost::text;
using namespace boost::text::detail;


// https://unicode.org/reports/tr9/#BD13
TEST(detail_bidi, compute_run_sequences)
{
    auto run_used = [](level_run r) { return r.used_; };

    // Using bidi_prop_t::L for all portions of the examples called "text".

    {
        props_and_embeddings_t paes = {
            // text1
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},

            {bidi_prop_t::RLE, false, 0},

            // text2
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},

            {bidi_prop_t::PDF, false, 1},

            {bidi_prop_t::RLE, false, 1},

            // text3
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},

            {bidi_prop_t::PDF, false, 1},

            // text4
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
        };

        auto const runs = find_all_runs(paes);

        EXPECT_EQ(runs.size(), 3u);
        EXPECT_TRUE(std::none_of(runs.begin(), runs.end(), run_used));
        EXPECT_EQ(runs[0].first_ - paes.begin(), 0);
        EXPECT_EQ(runs[0].last_ - paes.begin(), 4);
        EXPECT_EQ(runs[1].first_ - paes.begin(), 4);
        EXPECT_EQ(runs[1].last_ - paes.begin(), 13);
        EXPECT_EQ(runs[2].first_ - paes.begin(), 13);
        EXPECT_EQ(runs[2].last_ - paes.begin(), 16);

        auto const run_sequences = find_run_sequences(paes);

        EXPECT_EQ(run_sequences.size(), 3u);
        EXPECT_EQ(run_sequences[0].runs_[0].first_ - paes.begin(), 0);
        EXPECT_EQ(run_sequences[0].runs_[0].last_ - paes.begin(), 4);
        EXPECT_EQ(run_sequences[1].runs_[0].first_ - paes.begin(), 4);
        EXPECT_EQ(run_sequences[1].runs_[0].last_ - paes.begin(), 13);
        EXPECT_EQ(run_sequences[2].runs_[0].first_ - paes.begin(), 13);
        EXPECT_EQ(run_sequences[2].runs_[0].last_ - paes.begin(), 16);
    }

    {
        props_and_embeddings_t paes = {
            // text1
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},

            {bidi_prop_t::RLI, false, 0},

            // text2
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},

            {bidi_prop_t::PDI, false, 0},

            {bidi_prop_t::RLI, false, 0},

            // text3
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},

            {bidi_prop_t::PDI, false, 0},

            // text4
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
        };

        auto const runs = find_all_runs(paes);

        EXPECT_EQ(runs.size(), 5u);
        EXPECT_TRUE(std::none_of(runs.begin(), runs.end(), run_used));
        EXPECT_EQ(runs[0].first_ - paes.begin(), 0);
        EXPECT_EQ(runs[0].last_ - paes.begin(), 4);
        EXPECT_EQ(runs[1].first_ - paes.begin(), 4);
        EXPECT_EQ(runs[1].last_ - paes.begin(), 7);
        EXPECT_EQ(runs[2].first_ - paes.begin(), 7);
        EXPECT_EQ(runs[2].last_ - paes.begin(), 9);
        EXPECT_EQ(runs[3].first_ - paes.begin(), 9);
        EXPECT_EQ(runs[3].last_ - paes.begin(), 12);
        EXPECT_EQ(runs[4].first_ - paes.begin(), 12);
        EXPECT_EQ(runs[4].last_ - paes.begin(), 16);

        auto const run_sequences = find_run_sequences(paes);

        EXPECT_EQ(run_sequences.size(), 3u);
        EXPECT_EQ(run_sequences[0].runs_[0].first_ - paes.begin(), 0);
        EXPECT_EQ(run_sequences[0].runs_[0].last_ - paes.begin(), 4);
        EXPECT_EQ(run_sequences[0].runs_[1].first_ - paes.begin(), 7);
        EXPECT_EQ(run_sequences[0].runs_[1].last_ - paes.begin(), 9);
        EXPECT_EQ(run_sequences[0].runs_[2].first_ - paes.begin(), 12);
        EXPECT_EQ(run_sequences[0].runs_[2].last_ - paes.begin(), 16);
        EXPECT_EQ(run_sequences[1].runs_[0].first_ - paes.begin(), 4);
        EXPECT_EQ(run_sequences[1].runs_[0].last_ - paes.begin(), 7);
        EXPECT_EQ(run_sequences[2].runs_[0].first_ - paes.begin(), 9);
        EXPECT_EQ(run_sequences[2].runs_[0].last_ - paes.begin(), 12);
    }

    {
        props_and_embeddings_t paes = {
            // text1
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},

            {bidi_prop_t::RLI, false, 0},

            // text2
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},

            {bidi_prop_t::LRI, false, 1},

            // text3
            {bidi_prop_t::L, false, 2},
            {bidi_prop_t::L, false, 2},
            {bidi_prop_t::L, false, 2},

            {bidi_prop_t::RLE, false, 2},

            // text4
            {bidi_prop_t::L, false, 3},
            {bidi_prop_t::L, false, 3},
            {bidi_prop_t::L, false, 3},

            {bidi_prop_t::PDF, false, 3},

            // text5
            {bidi_prop_t::L, false, 2},
            {bidi_prop_t::L, false, 2},
            {bidi_prop_t::L, false, 2},

            {bidi_prop_t::PDI, false, 1},

            // text6
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},
            {bidi_prop_t::L, false, 1},

            {bidi_prop_t::PDI, false, 0},

            // text7
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::L, false, 0},
        };

        auto const runs = find_all_runs(paes);

        EXPECT_EQ(runs.size(), 7u);
        EXPECT_TRUE(std::none_of(runs.begin(), runs.end(), run_used));
        EXPECT_EQ(runs[0].first_ - paes.begin(), 0);
        EXPECT_EQ(runs[0].last_ - paes.begin(), 4);
        EXPECT_EQ(runs[1].first_ - paes.begin(), 4);
        EXPECT_EQ(runs[1].last_ - paes.begin(), 8);
        EXPECT_EQ(runs[2].first_ - paes.begin(), 8);
        EXPECT_EQ(runs[2].last_ - paes.begin(), 12);
        EXPECT_EQ(runs[3].first_ - paes.begin(), 12);
        EXPECT_EQ(runs[3].last_ - paes.begin(), 16);
        EXPECT_EQ(runs[4].first_ - paes.begin(), 16);
        EXPECT_EQ(runs[4].last_ - paes.begin(), 19);
        EXPECT_EQ(runs[5].first_ - paes.begin(), 19);
        EXPECT_EQ(runs[5].last_ - paes.begin(), 23);
        EXPECT_EQ(runs[6].first_ - paes.begin(), 23);
        EXPECT_EQ(runs[6].last_ - paes.begin(), 27);

        auto const run_sequences = find_run_sequences(paes);

        EXPECT_EQ(run_sequences.size(), 5u);
        EXPECT_EQ(run_sequences[0].runs_[0].first_ - paes.begin(), 0);
        EXPECT_EQ(run_sequences[0].runs_[0].last_ - paes.begin(), 4);
        EXPECT_EQ(run_sequences[0].runs_[1].first_ - paes.begin(), 23);
        EXPECT_EQ(run_sequences[0].runs_[1].last_ - paes.begin(), 27);
        EXPECT_EQ(run_sequences[1].runs_[0].first_ - paes.begin(), 4);
        EXPECT_EQ(run_sequences[1].runs_[0].last_ - paes.begin(), 8);
        EXPECT_EQ(run_sequences[1].runs_[1].first_ - paes.begin(), 19);
        EXPECT_EQ(run_sequences[1].runs_[1].last_ - paes.begin(), 23);
        EXPECT_EQ(run_sequences[2].runs_[0].first_ - paes.begin(), 8);
        EXPECT_EQ(run_sequences[2].runs_[0].last_ - paes.begin(), 12);
        EXPECT_EQ(run_sequences[3].runs_[0].first_ - paes.begin(), 12);
        EXPECT_EQ(run_sequences[3].runs_[0].last_ - paes.begin(), 16);
        EXPECT_EQ(run_sequences[4].runs_[0].first_ - paes.begin(), 16);
        EXPECT_EQ(run_sequences[4].runs_[0].last_ - paes.begin(), 19);
    }
}

TEST(detail_bidi, steps_W1_through_W7)
{
    // W1
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AL, false, 0},
            {bidi_prop_t::NSM, false, 0},
            {bidi_prop_t::NSM, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w1(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::NSM, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w1(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 1);
        EXPECT_EQ(it->prop_, bidi_prop_t::R);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::LRI, false, 0},
            {bidi_prop_t::NSM, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w1(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::LRI);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::PDI, true, 0},
            {bidi_prop_t::NSM, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w1(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::PDI);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AL, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::NSM, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::NSM, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w1(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 5);
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::NSM, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w1(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::R);
    }

    // W2
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AL, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w2(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AL, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w2(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::AL);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w2(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w2(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::R, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w2(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::R);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }

    // W4
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::ES, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w4(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::CS, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w4(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::CS, false, 0},
            {bidi_prop_t::AN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w4(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::ES, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w4(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 8);
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::CS, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w4(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 7);
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::CS, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::BN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w4(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 8);
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
    }

    // W5
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::ET, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::ET);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::ET, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ET);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::ET, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 5);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::BN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w5(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 6);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }

    // W6
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::ET, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::ES, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::CS, false, 0},
            {bidi_prop_t::AN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::AN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 2);
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }

    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::ET, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 4);
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::ES, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 5);
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::EN, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::CS, false, 0},
            {bidi_prop_t::AN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 4);
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::AN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::ET, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::AN, false, 0},
            {bidi_prop_t::BN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w6(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 4);
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::ON);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::AN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
    }

    // W7
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w7(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::R, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w7(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 3);
        EXPECT_EQ(it->prop_, bidi_prop_t::R);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::L, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w7(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 4);
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::L);
    }
    {
        props_and_embeddings_t paes = {
            {bidi_prop_t::R, false, 0},
            {bidi_prop_t::B, false, 0},
            {bidi_prop_t::BN, false, 0},
            {bidi_prop_t::EN, false, 0},
        };

        auto run_sequences = find_run_sequences(paes);
        EXPECT_EQ(run_sequences.size(), 1u);

        find_sos_eos(run_sequences, 1); // 1 implies sos==R
        w7(run_sequences[0]);

        auto it = run_sequences[0].begin();
        EXPECT_EQ(
            std::distance(run_sequences[0].begin(), run_sequences[0].end()), 4);
        EXPECT_EQ(it->prop_, bidi_prop_t::R);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::B);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::BN);
        ++it;
        EXPECT_EQ(it->prop_, bidi_prop_t::EN);
    }
}

// TODO: This is just here for early-stage testing.  Remove once an actual
// test exists for this function.
TEST(detail_bidi, bidirectional_order_instantiation)
{
    std::array<uint32_t, 1> str = {'a'};
    bidirectional_order(
        str.begin(), str.end(), TODO<std::array<uint32_t, 1>::iterator>{});
}
