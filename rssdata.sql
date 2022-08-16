SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

CREATE TABLE `sims_db` (
  `id` int(10) NOT NULL,
  `code` varchar(255) NOT NULL,
  `date` date DEFAULT NULL,
  `time` time DEFAULT NULL
) 
ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

ALTER TABLE `qrcodes`
  ADD PRIMARY KEY (`id`);

ALTER TABLE `qrcodes`
  MODIFY `id` int(10) NOT NULL AUTO_INCREMENT;
COMMIT;