const SimpleStorage = artifacts.require('./SimpleStorage.sol');

module.exports = async (deployer, network, accounts) => {
    await deployer.deploy(SimpleStorage);
    await deployer.ens.setAddress('simplestorage.eth', SimpleStorage.address, { from: accounts[0] })
    const simplesStorageInstance = await SimpleStorage.deployed();
    await simplesStorageInstance.addCamera(92834, { from: accounts[0] });
}
